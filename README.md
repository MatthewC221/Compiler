### Decaf compiler (MIT OCW 6.035)

The compiler has my own elements (both intentionally and unintentionally) to it because it is self-studied. 
Explanation of components

#### Scanner - syntax scanner that divides the text file into symbols, strings, integers and operands
#### Parser - mostly non contextual, analyses everything the scanner gives it and decides if everything makes sense within the context of that line. Using a stack and shifting + reducing (shift-reduce parser)
#### Semantic analyser - analyses the context of everything, void return in an integer function, string comparison with integer, variable exists, out of bounds array reference, etc...
Code generator - in the works...

##### RUNNING SCANNER/PARSER/SEMANTIC
*
chmod 755 semantic_test.pl
./semantic_test.pl 
*
##### Example: ./scanner parse new1 (where new1 is in parser_tests/)
![Alt text](/example.png?raw=true "Optional Title")
