# global-route-visualizer
Visualize route solutions from the ece 556 final project at UW-Madison

## setup
```
git clone https://github.com/phonyphonecall/global-route-visualizer.git
cd global-route-visualizer
make
```
## usage
`./visualizer <input_benchmark_filename> <solution_filename> <output_filename>`
 - `input_benchmark_filename`: The input benchmark. I.E. `adaptec1.gr`
 - `solution_filename`: Your solution after routing I.E. `my_output`
 - `output_filename`: The name of the html file the script will generate. I.E. `my_visual.html`

`open <output_filename>` ... open the visual in your favorite browser

## example
```
$ ./visualize ../ece556-global-routing/benchmarks/adaptec2.gr ../ece556-global-routing/out2.txt out2.html
total overflow: 54
$ open out2.html
```
example output for adaptec2.gr and a great routing run:


