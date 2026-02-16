# Verdant Souls

**Verdant Souls** is a 3D RPG prototype developed in **Unreal Engine 5**.
This project focuses on implementing core RPG combat systems, AI behavior, and modular gameplay mechanics using C++.

> ⚠️ Note: This repository primarily contains source code. Most large asset files are excluded to keep the repo lightweight.

---

## Project Overview

Verdant Souls is a combat-focused RPG prototype designed to showcase:

* Modular enemy systems
* Player combat mechanics
* Equipment & loot systems
* AI behavior logic
* Health & stamina management

The goal of this prototype is to build a scalable and extendable RPG foundation that can easily support additional enemies, weapons, and gameplay features.

---

## Core Mechanics Implemented

### Player Movement & Combat

* Third-person player movement
* Basic attack system
* Dodge mechanic
* Stamina-based action limitation
  
<img width="3824" height="1944" alt="HighresScreenshot00000" src="https://github.com/user-attachments/assets/ee2395fe-120d-4067-8e93-b477ff78a4f3" />

---

### Combat System

* Melee-based combat mechanics
* Damage calculation system
* Hit detection
* Enemy damage response
* Death handling logic
<img width="3824" height="1944" alt="HighresScreenshot00006" src="https://github.com/user-attachments/assets/170046fa-d0c4-43d9-b26e-ccd4d75b978b" />

---

### Equipment & Loot System

* Weapon equipping system
* Item pickup mechanics
* Enemy drop system:

  * 🟢 Souls
  * 🟡 Gold
* Modular structure for adding new items
<img width="3824" height="1944" alt="HighresScreenshot00001" src="https://github.com/user-attachments/assets/81fa6d31-ea56-4b20-ba02-a77170fb59ec" />

---

### Enemy AI System

* Patrol between predefined points
* Player detection using sight logic
* Chase behavior when player is detected
* Attack behavior when in range
* State-driven AI design

---

### Multiple Enemy Types

* Multiple regular enemy types
* One boss enemy
* Easily extendable enemy framework

  * Add new enemy classes with minimal changes
  * Scalable combat integration

---

### Health & Stamina System

* Player Health System
* Player Stamina System
* Stamina consumption on:

  * Dodge
* Death logic when health reaches zero

---

## Architecture & Design Goals

* Clean C++ structure
* Modular enemy base class
* Extendable combat framework
* Reusable components
* Designed for scalability

This prototype was built with long-term expansion in mind.

---

## Purpose of This Project

This prototype was developed to:

* Strengthen gameplay programming fundamentals
* Practice AI system implementation
* Build scalable RPG mechanics
* Showcase Unreal Engine C++ proficiency

---

## Future Improvements

* Expanded boss mechanics
* Skill tree system
* Inventory UI expansion
* Save & Load system
* Additional enemy archetypes
* Advanced stamina interactions
