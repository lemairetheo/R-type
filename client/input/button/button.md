# Documentation: `Button` Class

The `Button` class is an implementation of an interactive button using the SFML library. It includes a hover effect, smooth animation, and the ability to define custom actions via callbacks.

## Features

- **Hover effect**: The button slightly enlarges when hovered over.
- **Smooth animation**: A smooth transition between the `normal` and `hover` states.
- **Click callback**: Ability to define a function to be called on click.
- **Customization**:
  - Button text.
  - Colors for `normal`, `hover`, and `active` states.
  - Position and size.
- **Automatically centered text**.

---

## Initialization

### Constructor

```cpp
Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text,
       const sf::Color& textColor = sf::Color::Black,
       const sf::Color& normalColor = sf::Color::White,
       const sf::Color& hoverColor = sf::Color::Yellow);
```

### Parameters

| Parameter        | Type               | Description                                                    |
|------------------|--------------------|----------------------------------------------------------------|
| `position`       | `sf::Vector2f`     | Position of the center of the button.                         |
| `size`           | `sf::Vector2f`     | Size of the button (width x height).                          |
| `text`           | `std::string`      | Text displayed on the button.                                 |
| `textColor`      | `sf::Color`        | Text color. Default: `sf::Color::Black`.                      |
| `normalColor`    | `sf::Color`        | Background color for the normal state. Default: `sf::Color::White`. |
| `hoverColor`     | `sf::Color`        | Background color when hovered over. Default: `sf::Color::Yellow`. |

---

## Main Methods

### `handleEvent`

```cpp
void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
```

Handles SFML events to detect clicks or mouse hover.

- **Parameters**:
  - `event`: An SFML event (e.g., click or mouse movement).
  - `window`: The SFML window where the button is rendered.

---

### `render`

```cpp
void render(sf::RenderWindow& window);
```

Draws the button and its text in the window.

- **Parameters**:
  - `window`: The SFML window where the button is displayed.

---

### `setOnClick`

```cpp
void setOnClick(std::function<void()> callback);
```

Sets a callback function to be executed when the button is clicked.

- **Parameters**:
  - `callback`: A function with no parameters and no return value (`std::function<void()>`).

Example:

```cpp
button.setOnClick([]() {
    std::cout << "Button clicked!" << std::endl;
});
```

---

### `setColors`

```cpp
void setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& active);
```

Sets the colors for the different button states.

- **Parameters**:
  - `normal`: Color for the normal state.
  - `hover`: Color for the hover state.
  - `active`: Color for the active state.

---

### `setText`

```cpp
void setText(const std::string& text);
```

Changes the text displayed on the button.

- **Parameters**:
  - `text`: The text to display.

---

### `setPosition`

```cpp
void setPosition(const sf::Vector2f& position);
```

Moves the button to a new position.

- **Parameters**:
  - `position`: Position of the center of the button.

---

### `setSize`

```cpp
void setSize(const sf::Vector2f& size);
```

Changes the size of the button.

- **Parameters**:
  - `size`: New size (width x height).

---

## Example Usage

```cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include "input/button/Button.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Button Example");
    window.setFramerateLimit(60);

    // Creating the button
    ui::Button button(
        {400, 300}, {200, 50}, "Click Me", sf::Color::Black, sf::Color::White, sf::Color::Yellow
    );

    // Set an action on click
    button.setOnClick([]() {
        std::cout << "Button clicked!" << std::endl;
    });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle button events
            button.handleEvent(event, window);
        }

        // Rendering
        window.clear(sf::Color::Black);
        button.render(window);
        window.display();
    }

    return 0;
}
```

---

## Expected Result

1. An SFML window with a centered button.
2. The button changes size and color when hovered over.
3. When the button is clicked, the console displays **"Button clicked!"**.

---

## Notes

- **Custom Fonts**: Make sure the file `Arial.ttf` is available in the path `../client/asset/fonts/Arial.ttf`. You can use any other font by modifying the source file.
- **Extensibility**: The class can be extended to include other types of animations or interactions.