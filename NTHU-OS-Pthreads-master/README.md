# Producer-Consumer Problem

## Description
1. Reader thread read input file and move items to input queue.
2. 4 producers move items from input queue to worker queue.
3. Controller control how many consumers to work and consumer move items from worker queue to writer queue.
4. Writer move items from writer queue to output file.
   
## Run
1. Generate `transformer.cpp`: 
   ```
   python ./scripts/auto_gen_transformer.py --input input_file.json --output transformer.cpp
   ```
2. Build and run
   ```
   g++ -o main main.cpp transformer.cpp -lpthread
   ./main line_count input_file_path output_file_path
   ```
## Verify
* ```python ./script/verify --output output_file_path --answer anser_file_path```

