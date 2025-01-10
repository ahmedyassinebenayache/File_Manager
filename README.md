# README

## Introduction

This project implements a file management system in C, allowing storage and retrieval of student records in linked or contiguous storage formats. The code provides functionalities for file creation, searching, deletion, defragmentation, and other operations on the stored student records.

## Features

- **Linked Storage**:
  - Initialize, empty, and manage storage.
  - Create linked files (sorted and unsorted).
  - Add, delete, search, and display records in linked files.
  - Defragmentation of linked files.

- **Contiguous Storage**:
  - Initialize, empty, and manage storage.
  - Create contiguous files (sorted and unsorted).
  - Add, delete, search, and display records in contiguous files.
  - Defragmentation of contiguous files.

- **General Operations**:
  - Rename files.
  - Manage metadata blocks and allocation tables.

## Setup

1. **Prerequisites**:
   - GCC or any C compiler.
   - Basic understanding of C programming.

2. **Compilation**:
   Use the following command to compile the program:
   ```bash
   gcc -o file_management Test.c -lm
   ```
   The `-lm` flag links the math library required for some operations.

3. **Execution**:
   Run the compiled executable:
   ```bash
   ./file_management
   ```

## Usage

1. **Main Menu**:
   - Choose between Linked Storage, Contiguous Storage, or Exit.

2. **Linked Storage Options**:
   - Initialize, empty, create, display, search, or delete linked files.
   - Add students to sorted or unsorted files.
   - Defragment or perform logical/physical deletions.

3. **Contiguous Storage Options**:
   - Initialize, empty, create, display, search, or delete contiguous files.
   - Add students to sorted or unsorted files.
   - Defragment or perform logical/physical deletions.

4. **Example**:
   - Creating a linked file:
     - Input number of students, file name, and sorting preference.
     - Enter details for each student (ID, name, surname, section).

## Repository

The source code and related files for this project can be found in the GitHub repository:
[GitHub - File_Manager](https://github.com/ahmedyassinebenayache/File_Manager)

## Structure

- **`Tetudiant`**:
  Defines the student record structure with fields for ID, name, surname, section, and status.
- **`FDmeta`**:
  Manages metadata for each file, including size, storage type, and address.
- **Block Structures**:
  - `BLOC_ch`, `BLOC_co`, `BLOC_meta`, `BLOC_meta_ch`: Manage blocks of records or metadata.

## Contact

For questions or feedback, please contact the author at [ahmedyassinebenayache.acc@gmail.com].

