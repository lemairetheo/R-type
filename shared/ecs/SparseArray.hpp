/*
** EPITECH PROJECT, 2024
** R_typed
** File description:
** SparseArray
*/
#pragma once
#include <vector>
#include <optional>
#include <functional>
#include "IComponent.hpp"

namespace rtype {
    template<typename Component>
    class SparseArray : public IComponent{
    public:
        using value_type = std::optional<Component>;
        using reference_type = value_type&;
        using const_reference_type = const value_type&;
        using container_t = std::vector<value_type>;

        reference_type operator[](std::size_t idx) {
            if (idx >= _data.size())
                _data.resize(idx + 1);
            return _data[idx];
        }

        void insert_at(std::size_t idx, Component const& component) {
            if (idx >= _data.size())
                _data.resize(idx + 1);
            _data[idx] = component;
        }

        void erase(EntityID entity) override {
            if (entity < _data.size()) {
                _data[entity].reset();
            }
        }

        const_reference_type operator[](std::size_t idx) const {
            // if (idx >= _data.size())
            //     return std::nullopt;
            return _data[idx];
        }

        container_t& getData() { return _data; }

    private:
        container_t _data;
    };
}