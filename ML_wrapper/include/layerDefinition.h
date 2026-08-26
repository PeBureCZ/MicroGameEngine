#pragma once

#include "SFML/Graphics.hpp"

struct MlVerticesObject
{
    sf::VertexArray m_batch;
    sf::RenderStates m_state; //transform
};

using Z_POSITION = int64_t;
using LAYER_NUMBER = size_t;

struct MgeLayerObject
{
    Z_POSITION zPosition = 0;
    LAYER_NUMBER m_layer = 0;
    std::variant<MlVerticesObject, sf::Sprite, sf::Text> data = {}; // MlVerticesObject
};

struct MgeLayer
{
    LAYER_NUMBER m_layerNumber = 0;
    std::vector<std::shared_ptr<MgeLayerObject>> m_layerObjects;
};

using MGE_LAYERS = std::vector<MgeLayer>;