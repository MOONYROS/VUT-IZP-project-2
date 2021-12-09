# setcal.c
So this is my, or maybe I should say our first group project in the first degree of BUT FIT bachelor's degree.
<p> The basic motivation (except to pass the semester) was to learn how to work with data structures, reading and processing files in C. </p>
<p> The goal of this project was to implement all the functions from the subject of discrete mathematics to a single-file program. The functions included working with universum, sets, relations and commands which represent the functions of discrete mathematics (surjectivity, transitivity, reflexivity, etc.). </p>

<h2> Authors </h2>
<ul>
  <li> <a href="https://github.com/MOONYROS"> MOONYROS </a> (xlukas15)
  <li> <a href="https://github.com/Kumaros"> Kumaros </a> (xkouma02)
  <li> <a href="https://github.com/Petanna"> Petanna </a> (xdudov02)
  <li> <a href="https://github.com/mikkina"> mikkina </a> (xfukar00)
</ul>

<h1> Basic structure of the program </h1>
<p> In this part of the README, I would like to focus on describing how the program works. </p>

<h2> File input </h2>
<p> This program was supposed to take the name of the file in the argument section from the command line (that means in UNIX, the program was meant to be run like ./setcal FILENAME). I think that not much more description is needed for this part of the program...</p>

<h2> Data structures </h2>
<p> As it was one of the main goals, it should be no surprise that the first thing I started working on were the structures.
<br> There were 4 structures I used. 
<br> One was meant to contain all the information about a single line .txt file:
<ul>
  <li> contents - information about what is the first letter in the line. It could be 'U', 'S', 'R' or 'C'
  <li> *set - pointer to TWordList structure
  <li> *relation - pointer to TRelationItem structure
  <li> *command - pointer to TCommand structure
</ul>

<br> Then there were the other three structures that were aimed to work with a certain type of line (set, relation or command). These were recongnised by the first character (contents) in the line. To give a graphic explanation (which is in my opinion better for this case) I'll use a diagram model of these four structures.

<img src="https://raw.githubusercontent.com/MOONYROS/setcal/main/diagrams.jpg">

<h2> Basic explanation of the process </h2>
<p> First of all, I'll call the processFile function that basically reads the whole file and after that, all the lines are divided into the structures (each line could be set, relation or command) using the processLine function. </p>
<p> I'm also checking whether the first line in the text file has 'U' letter first. The 'U' tells us that it's a universe that always has to be on the first line of the file. To explain it a little bit more, its main purpose is to contain all the elements that are used in the file and it can also be used as a regular set, so that way user can work with it as well. </p>
<p> Then the program will process the other lines and after that it takes care of the commands using processCommands. To give a basic explanation of the commands, all of them have some sort of a result that they write on the stdout. Their result can be a truth value (written on output as true/false) (e.g. empty or bijective functions), a set (like domain or intersect) or a relation (closure_sym, closure_ref, closure_trans). Also, there is a bonus function that only prints one letter on the standard output. </p>
