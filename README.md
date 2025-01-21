# Cmplicity

Cmplicty (read as 'simplicity') aims to be a convenient tool to use regex and automatons for lexing purposes.

## How to run ?

The ```make``` command will build and create examples for the project.

## Automaton file syntax

```
AUTOMATON : FIELD ";" AUTOMATON | nil
FIELD : STATES_FIELD | FINALS_FIELD | INITIAL_FIELD | TRANSITION_FIELD
STATES_FIELD : "States:" STRING_ENUMERATION
FINALS_FIELD : "Finals:" STRING_ENUMERATION
INITIAL_FIELD : "Initial:" STRING
TRANSITION_FIELD : "Transitions:" TRANSITION_ENUMERATION
STRING_ENUMERATION : STRING "," STRING_ENUMERATION | nil
TRANSITION_ENUMERATION : TRANSITION "," TRANSITION_ENUMERATION | nil
TRANSITION : "(" STRING "," TERMINAL "," STRING ")"
TERMINAL : SINGLE_TERMINAL | "[" SINGLE_TERMINAL "-" SINGLE_TERMINAL "]"
SINGLE_TERMINAL : [^"," ")"]
STRING : SINGLE_TERMINAL STRING | nil

```

## Roadmap

1. Test thoroughly automaton loading from file
1. Add union, concatenation and star operations for automatons
1. Build automaton from regex
1. Do tokenization from regex
1. Implement a minimal language as proof of concept
