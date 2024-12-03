# Documentation : Classe `Button`

La classe `Button` est une implémentation d’un bouton interactif utilisant la bibliothèque SFML. Elle inclut un effet de survol (hover), une animation fluide, et la possibilité de définir des actions personnalisées via des callbacks.

## Fonctionnalités

- **Effet de hover** : Le bouton s'agrandit légèrement lorsqu'il est survolé.
- **Animation fluide** : Transition douce entre les états `normal` et `hover`.
- **Callback clic** : Possibilité de définir une fonction appelée lors d'un clic.
- **Personnalisation** :
    - Texte du bouton.
    - Couleurs pour les états `normal`, `hover` et `active`.
    - Position et taille.
- **Texte centré automatiquement**.

---

## Initialisation

### Constructeur

```cpp
Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text,
       const sf::Color& textColor = sf::Color::Black,
       const sf::Color& normalColor = sf::Color::White,
       const sf::Color& hoverColor = sf::Color::Yellow);
```

### Paramètres

| Paramètre       | Type               | Description                                                    |
|------------------|--------------------|----------------------------------------------------------------|
| `position`       | `sf::Vector2f`    | Position du centre du bouton.                                 |
| `size`           | `sf::Vector2f`    | Taille du bouton (largeur x hauteur).                         |
| `text`           | `std::string`     | Texte affiché sur le bouton.                                  |
| `textColor`      | `sf::Color`       | Couleur du texte. Par défaut : `sf::Color::Black`.            |
| `normalColor`    | `sf::Color`       | Couleur de fond à l'état normal. Par défaut : `sf::Color::White`. |
| `hoverColor`     | `sf::Color`       | Couleur de fond lorsqu'il est survolé. Par défaut : `sf::Color::Yellow`. |

---

## Méthodes principales

### `handleEvent`

```cpp
void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
```

Traite les événements SFML pour détecter les clics ou le survol de la souris.

- **Paramètres** :
    - `event` : Un événement SFML (par ex., clic ou mouvement de la souris).
    - `window` : La fenêtre SFML où le bouton est rendu.

---

### `render`

```cpp
void render(sf::RenderWindow& window);
```

Dessine le bouton et son texte dans la fenêtre.

- **Paramètres** :
    - `window` : La fenêtre SFML où le bouton est affiché.

---

### `setOnClick`

```cpp
void setOnClick(std::function<void()> callback);
```

Définit une fonction callback à exécuter lors d’un clic sur le bouton.

- **Paramètres** :
    - `callback` : Une fonction sans paramètres ni valeur de retour (`std::function<void()>`).

Exemple :

```cpp
button.setOnClick([]() {
    std::cout << "Bouton cliqué !" << std::endl;
});
```

---

### `setColors`

```cpp
void setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& active);
```

Définit les couleurs pour les différents états du bouton.

- **Paramètres** :
    - `normal` : Couleur pour l'état normal.
    - `hover` : Couleur pour l'état survolé.
    - `active` : Couleur pour l'état activé.

---

### `setText`

```cpp
void setText(const std::string& text);
```

Change le texte affiché sur le bouton.

- **Paramètres** :
    - `text` : Le texte à afficher.

---

### `setPosition`

```cpp
void setPosition(const sf::Vector2f& position);
```

Déplace le bouton à une nouvelle position.

- **Paramètres** :
    - `position` : Position du centre du bouton.

---

### `setSize`

```cpp
void setSize(const sf::Vector2f& size);
```

Modifie la taille du bouton.

- **Paramètres** :
    - `size` : Nouvelle taille (largeur x hauteur).

---

## Exemple d’utilisation

```cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include "inputs/Button.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Button Example");
    window.setFramerateLimit(60);

    // Création du bouton
    ui::Button button(
        {400, 300}, {200, 50}, "Click Me", sf::Color::Black, sf::Color::White, sf::Color::Yellow
    );

    // Définir une action sur clic
    button.setOnClick([]() {
        std::cout << "Bouton cliqué !" << std::endl;
    });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Gestion des événements du bouton
            button.handleEvent(event, window);
        }

        // Rendu
        window.clear(sf::Color::Black);
        button.render(window);
        window.display();
    }

    return 0;
}
```

---

## Résultat attendu

1. Une fenêtre SFML avec un bouton centré.
2. Le bouton change de taille et de couleur lorsqu'il est survolé.
3. Lors d’un clic sur le bouton, la console affiche **"Bouton cliqué !"**.

---

## Remarques

- **Polices personnalisées** : Assurez-vous que le fichier `Arial.ttf` est disponible dans le chemin `../client/asset/fonts/Arial.ttf`. Vous pouvez utiliser n'importe quelle autre police en modifiant le fichier source.
- **Extensibilité** : La classe peut être étendue pour inclure d'autres types d'animations ou d'interactions.
