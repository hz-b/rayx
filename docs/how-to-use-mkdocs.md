# Welcome to MkDocs

For full documentation visit [mkdocs.org](https://www.mkdocs.org).

## Commands

* `mkdocs new [dir-name]` - Create a new project.
* `mkdocs serve` - Start the live-reloading docs server.
* `mkdocs build` - Build the documentation site.
* `mkdocs -h` - Print help message and exit.

## Project layout

    mkdocs.yml    # The configuration file.
    docs/
        index.md  # The documentation homepage.
        ...       # Other markdown pages, images and other files.

## Test Stuff

### Lists

- Unordered
- List
    - Nested levels need 4 spaces
    
---

1. Ordered
2. List
    1. Next Level
    2. Hi mom

---

- [ ] Unchecked
- [x] Checked
    - [ ] Nested
    - [x] Works
    - [x] aswell

### Math

$$
\cos x=\sum_{k=0}^{\infty}\frac{(-1)^k}{(2k)!}x^{2k}
$$

### Emojis

:slight_smile: :rainbow_flag:

### Source code

- [x] Syntax Highlighting
- [x] Title
- [x] Annotations
- [x] Line numbers
- [x] Highlight lines


```cpp title="Bragg angle computation from Crystal.h" linenums="1" hl_lines="8-9"
/// @brief Computes the bragg angle
/// @param energy photonenergy of the ray
/// @param d lattice spacing*2
/// @param order Diffraction order
/// @return the bragg angle theta (rad)
RAYX_FN_ACC double getBraggAngle(double energy, double dSpacing2) {
    int order = 1;
    double wavelength = hvlam(energy);
    double theta_factor = (order * wavelength) / dSpacing2; // (1)!

    // Check for physical validity
    if (theta_factor > 1.0) {
        return -1.0;  // No reflection possible
    }

    double theta = asin(theta_factor);  // In radians
    return theta;
}
```

1.  :rainbow_flag: SCIENCE! With _inline_ `SCIENCE`  
    Some `Bragg` related math: $n \lambda = 2 d \sin \theta$
    
    Or as a separate block:
    $$
    n \lambda = 2 d \sin \theta
    $$
   

### Admonitions

!!! info "Admonitions"
    Admonitions are great!
    
??? example "Collapsible example"
    Collapsible Admonitions are nice too.
    ```cpp title="Bragg angle computation from Crystal.h"
    /// @brief Computes the bragg angle
    /// @param energy photonenergy of the ray
    /// @param d lattice spacing*2
    /// @param order Diffraction order
    /// @return the bragg angle theta (rad)
    RAYX_FN_ACC double getBraggAngle(double energy, double dSpacing2) {
        int order = 1;
        double wavelength = hvlam(energy);
        double theta_factor = (order * wavelength) / dSpacing2; // (1)!
    
        // Check for physical validity
        if (theta_factor > 1.0) {
            return -1.0;  // No reflection possible
        }
    
        double theta = asin(theta_factor);  // In radians
        return theta;
    }
    ```
    
    1. :rainbow_flag: SCIENCE! With _inline_ `code`

### Diagrams


```mermaid
graph LR
  A[Start] --> B{Failure?};
  B -->|Yes| C[Investigate...];
  C --> D[Debug];
  D --> B;
  B ---->|No| E[Success!];
```
