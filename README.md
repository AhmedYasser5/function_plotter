# Function Plotter

## ⛔ This project is being rewritten in C++ in [this repository](https://github.com/AhmedYasser5/Graphical_Calculator)

<p align="center">

  <img height="500" width="auto" src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/plotter.png" alt="Function Plotter">

</p>

## Features

* Using the open source GTK platform, meaning that the program can be configured to work on different operating systems.
* Plotting arbitrary polynomials containing only numbers, 'x', and operators ('+', '-', '*', '/', '^').
* Input validation and error handling. Explanations for different errors are provided in the [Examples section](#examples) below.
* Inspection of plots using the mouse. If you click and drag over a certain point, you will get its coordinates.

## Getting Started

To get started with the program, you need to compile it first. Compiling and running is as easy as running the following:

```shell
./compile_and_run.sh -r
```

OR

```shell
make RELEASE=1 run
```

You can also run compilation separately from running by creating an executable file first and then run it whenever needed:

```shell
# Creating an executable file
make RELEASE=1

# Running
./Function_Plotter_Release.exe
```

## Examples

<details><summary>Working Examples</summary>
<p align="center">

  <img src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/Working_Example1.gif" alt="Working Example 1">
  <br><br>
  <img src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/Working_Example2.gif" alt="Working Example 2">

</p>
</details>

<details><summary>Extra Features</summary>
<p align="center">

  <img src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/Extra_Features1.gif" alt="Extra Features 1">
  <br><br>
  <img src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/Extra_Features2.gif" alt="Extra Features 2">
  <br><br>
  <img src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/Extra_Features3.gif" alt="Extra Features 3">

</p>
</details>

<details><summary>Errors</summary>
<p align="center">

  <img src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/Errors1.gif" alt="Errors 1">
  <br><br>
  <img src="https://github.com/AhmedYasser5/function_plotter/blob/master/media/Errors2.gif" alt="Errors 2">

</p>
</details>
