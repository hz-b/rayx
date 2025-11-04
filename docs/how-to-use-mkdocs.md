# Writing Documentation with MkDocs

For full documentation have a look at [mkdocs.org](https://www.mkdocs.org) the [Material for MkDocs documentation](http://squidfunk.github.io/mkdocs-material/) and [MkDoxy](https://mkdoxy.kubaandrysek.cz/).

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

## API documentation with MkDoxy

This plugin uses `Doxygen` to generate markdown files from the code comments.
You can include parts of the API documentation as Snippets into the written guides using a [tag syntax](https://mkdoxy.kubaandrysek.cz/snippets/).

=== "Tag Syntax"
    ```yaml
    ::: doxy.rayx-core.class.method
    name: RAYX::BeamlineNode
    method: glm::mat4 getOrientation()
    ``` 
=== "Resulting Snippet"
::: doxy.rayx-core.Class.Method
name: RAYX::BeamlineNode
method: glm::dmat4 getOrientation()
indent_level: 4 


### Intellisense and errors

If there is an error inside the MkDoxy Tag it generates a helpful block with the error, available tags and the corresponding snippet.
For example using the tag without any config shows the available projects.

=== "Invalid Tag"
    ```yaml
    ::: doxy
    ```
=== "Error message"
::: doxy
indent_level: 4

This is especially helpful in the above example of a class method. Here the method signature is wrong, so a list of all class methods is displayed.

=== "Invalid Method Tag"
    ```yaml
    ::: doxy.rayxCore.class.method
    name: RAYX::BeamlineNode
    method: glm::mat4 getOrientation()
    ```
=== "Resulting Snippet"
::: doxy.rayxCore.Class.Method
name: RAYX::BeamlineNode
method: mat4 getOrientation()
indent_level: 4 

### Temporarily disabling MkDoxy

When working on documentation the local server takes some time to check and link the API documentation, even though the `Doxygen` output is cached to a `.mkdoxy` dir and only regenerated when necessary.
`MkDoxy` can be temporarily disabled with the environment variable `ENABLE_MKDOXY`

```sh
ENABLE_MKDOXY=false mkdocs serve
```

This is useful when writing documentation pages that don't use the API docs and you want to check the resulting page regularly, e.g. when using a lot of math.

## Markup

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


### Linking to other pages

Using markdown links **relative** to the current file (not the current page) this syntax is simply:

```md
[Home](index.md)
```

[Home](index.md)

### Images

There are some additional settings available for images that are not part of core markdown, like sizing and captions, see [Image Options](https://squidfunk.github.io/mkdocs-material/reference/images/).

![RAYX Logo](res/rayx-logo.png){ width="300" }
/// caption
RAYX Logo with a width of 300px
///
