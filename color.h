#pragma once

#include "tgaimage.h"

// Основные
constexpr TGAColor white   = {255, 255, 255, 255};
constexpr TGAColor black   = {  0,   0,   0, 255};
constexpr TGAColor gray    = {128, 128, 128, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor blue    = {255,   0,   0, 255};

// Яркие и вспомогательные
constexpr TGAColor yellow  = {  0, 255, 255, 255};
constexpr TGAColor cyan    = {255, 255,   0, 255};
constexpr TGAColor magenta = {255,   0, 255, 255};
constexpr TGAColor orange  = {  0, 165, 255, 255};
constexpr TGAColor purple  = {128,   0, 128, 255};
constexpr TGAColor pink    = {180, 105, 255, 255};

// Благородные / Металлические (пригодятся для тестов освещения)
constexpr TGAColor gold    = {  0, 215, 255, 255};
constexpr TGAColor silver  = {192, 192, 192, 255};
constexpr TGAColor bronze  = { 63, 127, 205, 255};

// Природные
constexpr TGAColor sky     = {250, 206, 135, 255}; // Небесно-голубой
constexpr TGAColor grass   = { 34, 139,  34, 255}; // Лесная зелень
constexpr TGAColor wood    = { 19,  69, 139, 255}; // Коричневый

// Темные оттенки (хороши для теней или заднего фона)
constexpr TGAColor dark_red   = {  0,   0, 139, 255};
constexpr TGAColor dark_blue  = {139,   0,   0, 255};
constexpr TGAColor dark_gray  = { 40,  40,  40, 255};