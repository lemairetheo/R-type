#ifndef RTYPECLIENTGRAPHICS_HPP
    #define RTYPECLIENTGRAPHICS_HPP


      #include <SFML/Graphics.hpp>
      #include <SFML/Window.hpp>
      #include <SFML/System.hpp>
      #include <string>
      #include <map>
      #include <vector>
      #include <memory>
      #include <iostream>

      class RTypeClientGraphics {
      public:
          RTypeClientGraphics(unsigned int width, unsigned int height, const std::string& title);

          void run();

      private:
          sf::RenderWindow window;
      };

#endif //RTYPECLIENTGRAPHICS_HPP
