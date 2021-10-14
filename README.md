# Linux system programming application

App that monitors vaccination status of citizens in every country using:
data strictures:
*Skip list
*bloom filter

technologies:
* linux signals for executable communication
* socket programming or pipe(FIFOS specifically) for data sharing between executables
* forked monitor executables to share load of citizen proccessing
* in 2nd app the monitors use mutli-threading producer consumer model to balance their load between threads

## Installation Instructions & Execution Instructions
- OPTIONAL: create your own virus and country files for the helloscript to generate the input file
- run the bash script with your input file to generate folder on data
- gcc or g++ required
- Navigate to each folder run `make` then run the executable
- run on linux
- issue commands to the app in the command line to manage the database
