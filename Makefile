example: examples/number.atm
	@./cmp automaton examples/number.atm 123.545
	@./cmp automaton examples/number.atm 1.14.5
	@./cmp automaton examples/number.atm coucou

examples/number.atm: cmp
	@mkdir -p examples
	@echo "States: IntegerPart, FloatPart;" > examples/number.atm
	@echo "Finals: IntegerPart, FloatPart;" >> examples/number.atm
	@echo "Initial: IntegerPart;" >> examples/number.atm
	@echo "Transitions:" >> examples/number.atm
	@echo "    (IntegerPart, [0-9], IntegerPart)," >> examples/number.atm
	@echo "    (IntegerPart, ., FloatPart)," >> examples/number.atm
	@echo "    (FloatPart, [0-9], FloatPart);" >> examples/number.atm

cmp: src/*.c
	@cc src/*.c -o cmp