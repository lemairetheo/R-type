# Documentation: `SelectBar` Class

The `SelectBar` class implements a slider UI element using the SFML library. It allows users to select a value between 0 and 100. The current value is displayed to the right of the slider, and the slider provides customization for colors, size, and appearance.

---

## Features

- **Value Selection**: Users can select a value between 0 and 100 by dragging the knob along the bar.
- **Real-Time Value Display**: The current value is displayed dynamically next to the slider.
- **Mouse Interaction**:
    - Hovering over the knob changes its color.
    - Clicking and dragging the knob updates the value in real-time.
- **Customizable Appearance**:
    - Colors for the slider, knob, hover state, and click state.
    - Background container with customizable color or texture.
- **Dynamic Position and Size**: The bar, knob, and value text adjust automatically to fit the configured size and position.

---

## Initialization

### Constructor

```cpp
SelectBar(const sf::Vector2f& position, const sf::Vector2f& size,
          const sf::Color& barColor = sf::Color::White,
          const sf::Color& knobColor = sf::Color::Green,
          const sf::Color& hoverColor = sf::Color::Yellow,
          const sf::Color& clickColor = sf::Color::Red,
          const sf::Color& containerColor = sf::Color(200, 200, 200));
```

### Parameters

| Parameter          | Type            | Description                                                       |
|---------------------|-----------------|-------------------------------------------------------------------|
| `position`          | `sf::Vector2f` | Position of the slider (top-left corner of the bar).             |
| `size`              | `sf::Vector2f` | Dimensions of the slider (width x height).                       |
| `barColor`          | `sf::Color`    | Background color of the slider bar. Default: `sf::Color::White`. |
| `knobColor`         | `sf::Color`    | Color of the knob. Default: `sf::Color::Green`.                  |
| `hoverColor`        | `sf::Color`    | Knob color when hovered over. Default: `sf::Color::Yellow`.      |
| `clickColor`        | `sf::Color`    | Knob color when clicked. Default: `sf::Color::Red`.              |
| `containerColor`    | `sf::Color`    | Background color of the container. Default: `sf::Color(200, 200, 200)`. |

---

## Key Methods

### `handleEvent`

```cpp
void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
```

Handles SFML events to manage interactions with the slider (e.g., clicking, dragging).

- **Parameters**:
    - `event`: An SFML event, such as a mouse click or movement.
    - `window`: The SFML window where the slider is rendered.

---

### `render`

```cpp
void render(sf::RenderWindow& window);
```

Draws the slider, including the bar, knob, and value text, in the specified window.

- **Parameters**:
    - `window`: The SFML window where the slider is displayed.

---

### `getValue`

```cpp
int getValue() const;
```

Returns the current value of the slider (an integer between 0 and 100).

---

### `setValue`

```cpp
void setValue(int value);
```

Sets the slider to a specific value (0-100). Values outside this range are clamped to fit within 0 and 100.

- **Parameters**:
    - `value`: The desired value for the slider.

---

### `setPosition`

```cpp
void setPosition(const sf::Vector2f& position);
```

Moves the slider to a new position.

- **Parameters**:
    - `position`: The new top-left position of the slider.

---

### `setSize`

```cpp
void setSize(const sf::Vector2f& size);
```

Resizes the slider. Adjusts the bar, knob, and text to fit the new dimensions.

- **Parameters**:
    - `size`: New dimensions for the slider (width x height).

---

### `setContainerColor`

```cpp
void setContainerColor(const sf::Color& color);
```

Changes the background color of the container surrounding the slider.

- **Parameters**:
    - `color`: The new background color.

---

### `setContainerTexture`

```cpp
void setContainerTexture(const sf::Texture& texture);
```

Applies a texture to the container surrounding the slider.

- **Parameters**:
    - `texture`: A reference to the texture to be used.

---

## Example Usage

```cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include "inputs/SelectBar.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SelectBar Example");
    window.setFramerateLimit(60);

    // Slider initialized at value 42
    ui::SelectBar selectBar(
        {200, 300}, {400, 20},
        sf::Color::White, sf::Color::Green, sf::Color::Yellow, sf::Color::Red, sf::Color(100, 100, 100)
    );
    selectBar.setValue(42); // Set initial value

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle slider events
            selectBar.handleEvent(event, window);
        }

        // Display the current slider value in the terminal
        std::cout << "Current Value: " << selectBar.getValue() << "\r" << std::flush;

        window.clear(sf::Color::Black);
        selectBar.render(window);
        window.display();
    }

    return 0;
}
```

---

## Expected Output

1. A slider with a white bar, green knob, and dark container appears at the specified position.
2. The value `42` is displayed to the right of the slider at startup.
3. When the knob is hovered over, it turns yellow; when clicked, it turns red.
4. Dragging the knob updates the value dynamically, which is printed to the terminal in real-time.

---

## Remarks

- **Font Requirements**: Ensure the file `Arial.ttf` is available at `../client/asset/fonts/Arial.ttf`. You can replace it with any other font by modifying the source code.
- **Custom Container**: Use `setContainerColor` or `setContainerTexture` to customize the container’s appearance.
- **Interaction States**:
    - Normal: The knob is displayed in its default color.
    - Hover: The knob changes to the hover color.
    - Click: The knob changes to the click color.
