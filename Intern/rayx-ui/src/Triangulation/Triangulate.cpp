#include "Triangulate.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <memory>
#include <queue>
#include <set>

#include "Colors.h"
#include "Debug/Debug.h"
#include "GeometryUtils.h"
#include "Shader/Constants.h"
#include "Triangulation/TraceTriangulation.h"

struct Point2D {
    double x;
    double y;

    int operator<(Point2D other) const { return y < other.y || (y == other.y && x > other.x); }
    int operator>(Point2D other) const { return y > other.y || (y == other.y && x < other.x); }

    int operator==(Point2D other) const { return x == other.x && y == other.y; }
};

// A simple polygon is a list of points in counter-clockwise order
typedef std::vector<uint32_t> PolygonSimple;

// A complex polygon is a list of simple polygons
// holes are represented by polygons in clockwise order
typedef std::vector<PolygonSimple> PolygonComplex;

typedef std::vector<uint32_t> Triangles;

enum VertexType {
    Start,
    Split,
    End,
    Merge,
    Regular,
};

// Vertex contains the corresponding TextureVertex/Point2D index and Vertex type (Start, Split, End, Merge, Regular)
// The VertexType is determines the behaviour of the sweep line algorithm
struct Vertex {
    uint32_t index;
    VertexType type;
};

struct EdgeEntry {
    Vertex origin;
    uint32_t prev;
    uint32_t next;
};

double absoluteAngle(const Point2D& p1, const Point2D& p2) { return atan2(p2.x - p1.x, p2.y - p1.y); }

VertexType toVertexType(const Point2D& prev, const Point2D& current, const Point2D& next) {
    double angle = atan2(next.x - current.x, next.y - current.y) - atan2(prev.x - current.x, prev.y - current.y);
    if (angle < 0) {
        angle += PI * 2;
    }
    if (prev < current && next < current) {
        return angle < PI ? Start : Split;
    } else if (prev > current && next > current) {
        return angle < PI ? End : Merge;
    } else {
        return Regular;
    }
}

enum ChainType {
    Left,
    Right,
};

// translate a possibly negative index to a positive one
int translateIndex(int index, int size) { return (index + size) % size; }

struct EdgeList {
    std::vector<EdgeEntry> edges;
    std::vector<Point2D> points;

    Point2D getOrigin(uint32_t edge) { return points[edges[edge].origin.index]; }

    uint32_t getOriginIndex(uint32_t edge) { return edges[edge].origin.index; }

    VertexType getOriginType(uint32_t edge) { return edges[edge].origin.type; }

    uint32_t getPrev(uint32_t edge) { return edges[edge].prev; }

    uint32_t getNext(uint32_t edge) { return edges[edge].next; }

    double xAtY(uint32_t edge, double y) {
        Point2D p1 = getOrigin(edge);
        Point2D p2 = getOrigin(getNext(edge));

        if (p1.y == p1.y) {
            return (p1.x + p2.x) / 2;
        }

        return p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
    }

    EdgeList(const PolygonComplex& poly, std::vector<TextureVertex>& verts) {
        points.reserve(verts.size());
        for (auto& vert : verts) {
            points.push_back({-vert.pos.x, vert.pos.z});
        }
        uint32_t startI = 0;
        for (auto& simple : poly) {
            int simpleSize = static_cast<int>(simple.size());
            for (int i = 0; i < simpleSize; i++) {
                int prevI = translateIndex(i - 1, simpleSize);
                int currentI = i;
                int nextI = translateIndex(i + 1, simpleSize);
                VertexType type = toVertexType(points[simple[prevI]], points[simple[currentI]], points[simple[nextI]]);
                edges.push_back(EdgeEntry{{simple[currentI], type}, startI + static_cast<uint32_t>(prevI), startI + static_cast<uint32_t>(nextI)});
            }
            startI += static_cast<uint32_t>(simpleSize);
        }
    }

    // insert an edge from v to w and from w to v
    std::pair<uint32_t, uint32_t> split(uint32_t v_i, uint32_t w_i) {
        EdgeEntry v = edges[v_i];
        EdgeEntry w = edges[w_i];
        edges.push_back(EdgeEntry{v.origin, v.prev, w_i});
        uint32_t vw = static_cast<uint32_t>(edges.size()) - 1;
        edges.push_back(EdgeEntry{w.origin, w.prev, v_i});
        uint32_t wv = static_cast<uint32_t>(edges.size()) - 1;
        edges[edges[v_i].prev].next = vw;
        edges[v_i].prev = wv;
        edges[edges[w_i].prev].next = wv;
        edges[w_i].prev = vw;
        return {vw, wv};
    }

    // merge the left and right chain into a sorted sequence
    std::vector<std::pair<uint32_t, ChainType>> toSortedSequence(uint32_t top_edge) {
        std::vector<std::pair<uint32_t, ChainType>> result;
        uint32_t currentLeft = getNext(top_edge);
        uint32_t currentRight = getPrev(top_edge);
        result.push_back(std::make_pair(top_edge, Left));
        while (currentLeft != currentRight) {
            if (getOrigin(currentLeft) > getOrigin(currentRight)) {
                result.push_back(std::make_pair(currentLeft, Left));
                currentLeft = getNext(currentLeft);
            } else {
                result.push_back(std::make_pair(currentRight, Right));
                currentRight = getPrev(currentRight);
            }
        }
        result.push_back(std::make_pair(currentLeft, Left));
        return result;
    }

    // A sweep line algorithm to partition a polygon into monotone pieces
    void makeMonotone() {
        double y;  // current y coordinate of the sweep line
        // set of edges intersecting the sweep line, ordered by x coordinate
        auto cmp_set = [&y, this](uint32_t a, uint32_t b) { return xAtY(a, y) < xAtY(b, y); };
        auto set = std::set<uint32_t, decltype(cmp_set)>(cmp_set);

        auto getLeftEdge = [&set](uint32_t edge) {
            auto it = set.lower_bound(edge);
            if (it == set.begin()) {
                throw std::runtime_error("Triangulate: invalid polygon");
            }
            it--;
            return *it;
        };

        // priority queue of edges, ordered by y coordinate of the origin
        auto cmp_prio = [this](uint32_t a, uint32_t b) { return getOrigin(a) < getOrigin(b); };
        auto priority_queue = std::priority_queue<uint32_t, std::vector<uint32_t>, decltype(cmp_prio)>(cmp_prio);

        auto helper = std::map<uint32_t, uint32_t>();

        for (uint32_t i = 0; i < edges.size(); i++) {
            priority_queue.push(i);
        }

        while (!priority_queue.empty()) {
            auto edge = priority_queue.top();

            priority_queue.pop();
            y = getOrigin(edge).y;

            switch (getOriginType(edge)) {
                case Start: {
                    set.insert(edge);
                    helper[edge] = edge;
                    break;
                }
                case End: {
                    uint32_t prev = getPrev(edge);
                    if (getOriginType(helper[prev]) == Merge) {
                        split(edge, helper[prev]);
                    }
                    helper.erase(prev);
                    set.erase(prev);
                    break;
                }
                case Split: {
                    auto left_edge = getLeftEdge(edge);
                    auto diags = split(edge, helper[left_edge]);
                    helper[left_edge] = diags.first;
                    set.insert(edge);
                    helper[edge] = edge;
                    break;
                }
                case Merge: {
                    uint32_t prev = getPrev(edge);
                    if (getOriginType(helper[prev]) == Merge) {
                        split(edge, helper[prev]);
                    }
                    helper.erase(prev);
                    set.erase(prev);
                    auto left_edge = getLeftEdge(edge);
                    if (getOriginType(helper[left_edge]) == Merge) {
                        auto diags = split(edge, helper[left_edge]);
                        helper[left_edge] = diags.first;
                    } else {
                        helper[left_edge] = edge;
                    }
                    break;
                }
                case Regular: {
                    bool isLeft = getOrigin(edge) < getOrigin(getPrev(edge));
                    if (isLeft) {
                        uint32_t prev = getPrev(edge);
                        if (getOriginType(helper[prev]) == Merge) {
                            split(edge, helper[prev]);
                        }
                        helper.erase(prev);
                        set.erase(prev);
                        set.insert(edge);
                        helper[edge] = edge;
                    } else {
                        auto left_edge = getLeftEdge(edge);
                        if (getOriginType(helper[left_edge]) == Merge) {
                            auto diags = split(edge, helper[left_edge]);
                            helper[left_edge] = diags.first;
                        } else {
                            helper[left_edge] = edge;
                        }
                    }
                    break;
                }
            }
        }
    }

    bool checkAngle(ChainType type, uint32_t current, uint32_t last, uint32_t back) {
        if (type == Left) {
            return absoluteAngle(getOrigin(current), getOrigin(last)) < absoluteAngle(getOrigin(current), getOrigin(back));
        } else {
            return absoluteAngle(getOrigin(current), getOrigin(last)) > absoluteAngle(getOrigin(current), getOrigin(back));
        }
    }

    void triangulateMonotone(uint32_t top_edge) {
        auto sortedSequence = toSortedSequence(top_edge);
        std::vector<std::pair<uint32_t, ChainType>> stack;
        stack.push_back(sortedSequence[0]);
        stack.push_back(sortedSequence[1]);

        for (uint32_t i = 2; i < sortedSequence.size() - 1; i++) {
            auto current = sortedSequence[i];
            auto last = stack.back();
            stack.pop_back();
            if (current.second == Left) {
                if (last.second == Left) {
                    while (!stack.empty() && checkAngle(Left, current.first, last.first, stack.back().first)) {
                        auto diag = split(current.first, stack.back().first);
                        last.first = diag.second;
                        stack.pop_back();
                    }
                    stack.push_back(last);
                    stack.push_back(current);
                } else {
                    uint32_t currentInside = current.first;
                    auto beforeCurrent = last;
                    bool firstIteration = true;
                    while (stack.size() > 0) {
                        auto diag = split(currentInside, last.first);
                        last = stack.back();
                        stack.pop_back();
                        currentInside = diag.first;
                        if (firstIteration) {
                            firstIteration = false;
                            beforeCurrent.first = diag.second;
                        }
                    }
                    stack.push_back(beforeCurrent);
                    stack.push_back(current);
                }
            } else {
                if (last.second == Right) {
                    while (!stack.empty() && checkAngle(Right, current.first, last.first, stack.back().first)) {
                        auto diag = split(current.first, stack.back().first);
                        last = stack.back();
                        stack.pop_back();
                        current.first = diag.first;
                    }
                    stack.push_back(last);
                    stack.push_back(current);
                } else {
                    auto currentOutside = current;
                    auto lastCopy = last;
                    bool firstIteration = true;
                    while (stack.size() > 0) {
                        auto diag = split(current.first, last.first);
                        last = stack.back();
                        stack.pop_back();
                        if (firstIteration) {
                            firstIteration = false;
                            currentOutside.first = diag.first;
                        }
                    }
                    stack.push_back(lastCopy);
                    stack.push_back(currentOutside);
                }
            }
        }
        auto bottom = sortedSequence.back();
        stack.pop_back();
        auto last = stack.back();
        stack.pop_back();
        if (last.second == Left) {
            while (!stack.empty()) {
                split(bottom.first, last.first);
                last = stack.back();
                stack.pop_back();
            }
        } else {
            while (!stack.empty()) {
                auto diag = split(bottom.first, last.first);
                bottom.first = diag.first;
                last = stack.back();
                stack.pop_back();
            }
        }
    }

    bool isTop(uint32_t edge) { return getOrigin(getPrev(edge)) < getOrigin(edge) && getOrigin(getNext(edge)) < getOrigin(edge); }
};

void triangulate(const PolygonComplex& poly, std::vector<TextureVertex>& points, std::vector<uint32_t>& indices) {
    auto edge_list = EdgeList(poly, points);
    edge_list.makeMonotone();
    std::vector<uint32_t> tops;
    for (uint32_t edge = 0; edge < edge_list.edges.size(); edge++) {
        if (edge_list.isTop(edge)) {
            tops.push_back(edge);
        }
    }
    for (uint32_t top : tops) {
        edge_list.triangulateMonotone(top);
    }
    for (uint32_t edge = 0; edge < edge_list.edges.size(); edge++) {
        if (edge_list.isTop(edge)) {
            indices.push_back(edge_list.getOriginIndex(edge_list.getPrev(edge)));
            indices.push_back(edge_list.getOriginIndex(edge));
            indices.push_back(edge_list.getOriginIndex(edge_list.getNext(edge)));
        }
    }
}

// ------ Helper functions ------

// Cutout to Outline conversion
// Holes are represented by polygons in clockwise order
PolygonSimple calculateOutlineFromCutout(const Cutout& cutout, std::vector<TextureVertex>& vertices, bool clockwise = false) {
    constexpr double defWidthHeight = 27.3f;
    Outline outline;

    switch (static_cast<int>(cutout.m_type)) {
        case CTYPE_TRAPEZOID: {
            TrapezoidCutout trapezoid = deserializeTrapezoid(cutout);
            outline.calculateForQuadrilateral(trapezoid.m_widthA, trapezoid.m_widthB, trapezoid.m_length, trapezoid.m_length);
            break;
        }
        case CTYPE_RECT: {
            RectCutout rect = deserializeRect(cutout);
            outline.calculateForQuadrilateral(rect.m_width, rect.m_width, rect.m_length, rect.m_length);
            break;
        }
        case CTYPE_ELLIPTICAL: {
            EllipticalCutout ellipse = deserializeElliptical(cutout);
            outline.calculateForElliptical(ellipse.m_diameter_x, ellipse.m_diameter_z);
            break;
        }
        case CTYPE_UNLIMITED:
        default: {
            outline.calculateForQuadrilateral(defWidthHeight, defWidthHeight, defWidthHeight, defWidthHeight);
            break;
        }
    }
    uint32_t offset = static_cast<uint32_t>(vertices.size());
    vertices.insert(vertices.end(), outline.vertices.begin(), outline.vertices.end());
    PolygonSimple indices;

    uint32_t outlineSize = static_cast<uint32_t>(outline.vertices.size());

    for (uint32_t i = 0; i < outlineSize; i++) {
        if (clockwise) {
            indices.push_back(offset + outlineSize - i - 1);  // Reverse the order
        } else {
            indices.push_back(offset + i);
        }
    }
    return indices;
}

void planarTriangulation(const RAYX::OpticalElement& element, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices) {
    // The slit behaviour needs special attention, since it is basically three cutouts (the slit, the beamstop and the opening)
    PolygonComplex poly;
    if (element.m_element.m_behaviour.m_type == BTYPE_SLIT) {
        SlitBehaviour slit = deserializeSlit(element.m_element.m_behaviour);
        poly.push_back(calculateOutlineFromCutout(slit.m_beamstopCutout, vertices));
        poly.push_back(calculateOutlineFromCutout(element.m_element.m_cutout, vertices));
        poly.push_back(calculateOutlineFromCutout(slit.m_openingCutout, vertices, true));  // Hole -> Clockwise order
    } else {
        poly.push_back(calculateOutlineFromCutout(element.m_element.m_cutout, vertices));
    }
    triangulate(poly, vertices, indices);
}

bool isPlanar(const QuadricSurface& q) { return (q.m_a11 == 0 && q.m_a22 == 0 && q.m_a33 == 0) && (q.m_a14 != 0 || q.m_a24 != 0 || q.m_a34 != 0); }

// ------ Interface functions ------

/**
 * This function takes optical elements and categorizes them for efficient triangulation.
 */
void triangulateObject(const RAYX::OpticalElement& element, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices) {
    switch (static_cast<int>(element.m_element.m_surface.m_type)) {
        case STYPE_PLANE_XZ: {
            planarTriangulation(element, vertices, indices);
            break;
        }
        case STYPE_QUADRIC: {
            QuadricSurface q = deserializeQuadric(element.m_element.m_surface);
            if (isPlanar(q)) {
                planarTriangulation(element, vertices, indices);
            } else {
                traceTriangulation(element, vertices, indices);
            }
            break;
        }
        case STYPE_TOROID: {
            traceTriangulation(element, vertices, indices);
            break;
        }
        default:
            RAYX_ERR << "Unknown element type: " << element.m_element.m_surface.m_type;
            break;
    }
}
