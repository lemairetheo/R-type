# Documentation: `TextInput` Class

The `TextInput` class is an implementation of a text input field using the SFML library. It supports user input, placeholder text, real-time text change callbacks, and customizable colors for various elements. Placeholder text is specified directly in the constructor for easier initialization.

---

## Features

- **Real-time Text Updates**: Retrieve the current input text via a callback when it changes.
- **Mouse Click Activation**: Activates the input field when clicked.
- **Placeholder Support**: Displays placeholder text when the input is empty and not focused, specified directly in the constructor.
- **Customizable Appearance**:
  - Text color.
  - Box color.
  - Placeholder text and color.
- **Dynamic Size and Position**: Automatically adjusts the text position when resized or moved.

---

## Initialization

### Constructor

```cpp
TextInput(const sf::Vector2f& position, const sf::Vector2f& size, 
          const sf::Color& textColor = sf::Color::Black,
          const sf::Color& boxColor = sf::Color::White,
          const sf::Color& placeholderColor = sf::Color(150, 150, 150),
          const std::string& placeholder = "Enter text...");
```

### Parameters

| Parameter          | Type            | Description                                                       |
|---------------------|-----------------|-------------------------------------------------------------------|
| `position`          | `sf::Vector2f` | Position of the input box.                                       |
| `size`              | `sf::Vector2f` | Dimensions of the input box (width x height).                    |
| `textColor`         | `sf::Color`    | Color of the entered text. Default: `sf::Color::Black`.          |
| `boxColor`          | `sf::Color`    | Background color of the input box. Default: `sf::Color::White`.  |
| `placeholderColor`  | `sf::Color`    | Color of the placeholder text. Default: `sf::Color(150, 150, 150)`.|
| `placeholder`       | `std::string`  | Placeholder text displayed when the input is empty. Default: `"Enter text..."`. |

---

## Key Methods

### `handleEvent`

```cpp
void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
```

Handles SFML events to process mouse clicks and keyboard input.

- **Parameters**:
  - `event`: An SFML event (e.g., text entry or mouse click).
  - `window`: The SFML window where the input is rendered.

---

### `render`

```cpp
void render(sf::RenderWindow& window);
```

Draws the input box, entered text, or placeholder text in the specified window.

- **Parameters**:
  - `window`: The SFML window where the input is displayed.

---

### `setOnTextChanged`

```cpp
void setOnTextChanged(std::function<void(const std::string&)> callback);
```

Defines a callback function that is triggered whenever the text changes.

- **Parameters**:
  - `callback`: A function taking the current text as a parameter (`std::function<void(const std::string&)>`).

Example:

```cpp
textInput.setOnTextChanged([](const std::string& text) {
    std::cout << "Current text: " << text << std::endl;
});
```

---

### `setPosition`

```cpp
void setPosition(const sf::Vector2f& position);
```

Moves the input box to a new position.

- **Parameters**:
  - `position`: The new position of the input box.

---

### `setSize`

```cpp
void setSize(const sf::Vector2f& size);
```

Resizes the input box.

- **Parameters**:
  - `size`: New dimensions of the input box (width x height).

---

### `getValue`

```cpp
std::string getValue() const;
```

Returns the current input text.

---

## Example Usage

```cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include "input/TextInput/TextInput.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "TextInput Example");
    window.setFramerateLimit(60);

    // Create a TextInput field with a placeholder
    ui::TextInput textInput(
        {200, 200}, {400, 50}, 
        sf::Color::Black, sf::Color::White, sf::Color(150, 150, 150), 
        "Type something..."
    );

    // Define a callback for real-time text updates
    textInput.setOnTextChanged([](const std::string& text) {
        std::cout << "Current text: " << text << std::endl;
    });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            textInput.handleEvent(event, window);
        }

        window.clear(sf::Color::Black);
        textInput.render(window);
        window.display();
    }

    return 0;
}
```

---

## Expected Output

1. A white input box appears at the specified position in the window.
2. The placeholder text "Type something..." is visible when the input is empty.
3. Clicking on the input box activates it, allowing you to type.
4. The console prints the current text in real-time whenever it changes.

---

## Remarks

- **Font Requirements**: Ensure that the file `Arial.ttf` is available at `../client/asset/fonts/Arial.ttf`. You can replace it with any other font by modifying the source code.
- **Real-Time Updates**: Use the `setOnTextChanged` method to process input dynamically.
