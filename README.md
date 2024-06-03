# LRU Cache Simulator

## Overview

This project is a Least Recently Used (LRU) cache simulator implemented in C. The simulator is designed to mimic the behavior of a hardware cache using the LRU replacement policy, which discards the least recently used items first when the cache is full.

## Table of Contents

1. [Introduction](#introduction)
2. [Features](#features)
3. [Installation](#installation)
4. [Usage](#usage)
5. [Examples](#examples)
6. [File Structure](#file-structure)
7. [Contributing](#contributing)
8. [License](#license)

## Introduction

Caching is a critical concept in computer architecture and systems design, used to speed up data access by storing frequently accessed data in a faster, but smaller, storage medium. This project provides a simulation of an LRU cache, demonstrating how the LRU replacement policy operates and its effects on cache performance.

## Features

- **LRU Replacement Policy**: Implements the LRU policy to manage cache contents.
- **Simulation of Cache Hits and Misses**: Tracks and reports cache hits and misses.
- **Configurable Cache Parameters**: Allows customization of cache size and block size.
- **Detailed Output**: Provides detailed simulation output for analysis.

## Installation

To compile the LRU cache simulator, you will need a C compiler such as `gcc`. Follow the steps below to set up the project:

1. Clone the repository:
    ```sh
    git clone https://github.com/dagmawiazerihun/LRU-Cache-Simulator.git
    cd LRU-Cache-Simulator
    ```

2. Compile the C source file:
    ```sh
    gcc -o csim3 csim3.c
    ```

## Usage

To run the simulator, use the following command:
```sh
./csim3 <cache_size> <block_size> <trace_file>
