# Documentation: `TextInput` Class

The `TextInput` class is an implementation of a text input field using the SFML library. It supports user input, placeholder text, real-time text change callbacks, customizable colors, and input modes (text, numbers, or both). Placeholder text and input mode are specified directly in the constructor for easier initialization.

---

## Features

- **Input Modes**: Accept text, numbers, or any alphanumeric characters with specific symbols (`;`, `:`, `.`, `/`, `\\`).
- **Real-time Text Updates**: Retrieve the current input text via a callback when it changes.
- **Mouse Click Activation**: Activates the input field when clicked.
- **Placeholder Support**: Displays placeholder text when the input is empty and not focused.
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
          const std::string& placeholder = "Enter text...",
          InputMode mode = InputMode::Any);
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
| `mode`              | `InputMode`    | Mode of input. Default: `InputMode::Any`.                        |

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
#include "inputs/TextInput.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "TextInput Example");
    window.setFramerateLimit(60);

    // Input field for text
    ui::TextInput textInput(
        {200, 200}, {400, 50}, 
        sf::Color::Black, sf::Color::White, sf::Color(150, 150, 150), 
        "Enter text...", ui::TextInput::InputMode::Text
    );

    // Input field for numbers
    ui::TextInput numberInput(
        {200, 300}, {400, 50}, 
        sf::Color::Black, sf::Color::White, sf::Color(150, 150, 150), 
        "Enter numbers...", ui::TextInput::InputMode::Numbers
    );

    // Define callbacks for real-time updates
    textInput.setOnTextChanged([](const std::string& text) {
        std::cout << "Text Input: " << text << std::endl;
    });

    numberInput.setOnTextChanged([](const std::string& text) {
        std::cout << "Number Input: " << text << std::endl;
    });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            textInput.handleEvent(event, window);
            numberInput.handleEvent(event, window);
        }

        window.clear(sf::Color::Black);
        textInput.render(window);
        numberInput.render(window);
        window.display();
    }

    return 0;
}
```

---

## Expected Output

1. Two white input boxes appear at the specified positions in the window.
2. The placeholder text "Enter text..." and "Enter numbers..." are visible when the inputs are empty.
3. Clicking on an input box activates it, allowing you to type.
4. The first input accepts alphanumeric characters and symbols (`;`, `:`, `.`, `/`, `\`).
5. The second input accepts only numeric values.
6. The console prints the current input text in real-time for both fields.

---

## Remarks

- **Font Requirements**: Ensure that the file `Arial.ttf` is available at `../client/asset/fonts/Arial.ttf`. You can replace it with any other font by modifying the source code.
- **Real-Time Updates**: Use the `setOnTextChanged` method to process input dynamically.
- **Input Modes**:
  - `InputMode::Text`: Accepts letters, numbers, and some symbols (`;`, `:`, `.`, `/`, `\`).
  - `InputMode::Numbers`: Accepts only digits.
  - `InputMode::Any`: Accepts any characters.
