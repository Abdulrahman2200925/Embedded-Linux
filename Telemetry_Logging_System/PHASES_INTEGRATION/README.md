# Cpp Project: A Telemetry & Logging System
> This project is a scalable, multi-threaded logging system written in C++; designed with best architectural and implementation practices in mind, while utilizing both the power Modern C++ provides in addition to industry-standard libraries and tools. 

## Features
* Support for multiple local/remote sources of data.
* Intuitive data formatting.
* Support for multiple data storage strategies that can be accessed later.

## System Overview
![](./README_Photos/project_bd.svg)

## Topics
### Modern C++
* Dynamic Dispatch (Virtual Functions + Virtual Destructors).
* Casting (Static & Dynamic + RTTI).
* Copy Semantics.
* Move Semantics & Ownership Transfer.
* Threading, Concurrency, & Callables.
* Smart Pointers.
* Special Member Functions & Rules of 0/3/5.
* Templates & Generic Programming.

### Libraries
* `vsomeip`
* `magic_enum`
* `CommonAPI`, `CommonAPI-SomeIP` (TBD)
* `dlt` (TBD)

### Design Patterns
* Creational Patterns &rarr; Factory, Builder, Creational.
* Structural Patterns &rarr; Proxy, Adapter, Facade.
* Behavioural Patterns &rarr; Strategy, Observer.
* Concurrency Patterns &rarr; Thread Pool.

## Grading & Rewards
> The project consists of 6 phases; each phase contains some mandatory requirements and some extra ones for a more challenging and rewarding experience. Each of the following items is rewarded with points as follows (per phase):
* Mandatory phase requirements (implementation w/ C++ as detailed in the design) &rarr; 50 Points.
* Implement listed design patterns &rarr; 125 Points.
* Perform unit test for each class &rarr; 100 Points.
* Build using CMake (each module is built as a library and linked with main app) &rarr; 75 Points.
* Build using Bazel (each module is built as a library and linked with main app) &rarr; 75 Points.
* Documentation (Markdown + class/sequence diagrams using PlantUML) &rarr; 100 Points.
* Bonus challenges ( ͡° ͜ʖ ͡°) &rarr; 200 Points. 

> Rewards would be a surprise for now :))
