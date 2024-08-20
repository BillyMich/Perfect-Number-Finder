# Perfect Number Finder

This program is designed to find perfect numbers using multiple threads in C. A perfect number is a positive integer that is equal to the sum of its proper divisors (excluding the number itself). For example, 6 is a perfect number because its proper divisors (1, 2, 3) sum up to 6.

## Prerequisites

To compile and run this program, you need to have a C compiler installed on your system, such as GCC.

## Usage

1. Clone the repository or download the source code file `perfectNumberFinder.c`.
2. Open a terminal and navigate to the directory containing the source code file.
3. Compile the program using the following command:
    ```
    gcc -O3 -pthread -o perfectNumberFinder perfectNumberFinder.c -lm

4. Run the program by executing the generated binary file:
    ```
    ./perfectNumberFinder
    ```
5. Enter the number of threads you want to use for finding perfect numbers.
6. The program will start running and display the perfect numbers it finds along with the time taken to find them.

## Customization

- You can modify the `MAX_THREADS` constant to change the maximum number of threads the program can use.
- The `QUEUE_SIZE` constant determines the size of the task queue. You can adjust it based on your requirements.

## Contributing

Contributions to this project are welcome. If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.

## License

This program is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.