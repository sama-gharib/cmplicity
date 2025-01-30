# Cmplicity

Cmplicty (read as 'simplicity') aims to be a convenient tool to use regex and automatons for lexing purposes.

## How to run ?

The ```make``` command will build and create examples for the project.

## Beware

Currently, regex does not support the '^' range negation operator.
It would not be complicated to but it would require compiling a simple regex such as ```[^m]``` to an automaton with
as much transitions as there is printable characters 
in the ASCII table (minus one). Such automaton would
greatly slow down the ```Match``` function since non-determinism
is quiet expensive. However, writing ```([a-l]|[n-z])``` compiles down to
an automaton with only two transitions (see the ```Transition``` structure definition in ```automaton.h```)

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

1. Do tokenization from regex
1. Implement a minimal language as proof of concept
