# automaid

This program convert raw data transmitted by Mermaid instruments to classify datas, correct clock drifts, interpolate float position and then generate seismic SAC files, plot seismic events and dives and generate KML files.

### 1. INSTALLATION

This installation procedure have been tested with macOS. For Linux the
procedure is valid but one could prefer to use the package manager.
For Windows the installation of python 2.7 is valid but the compilation
of the wavelet inversion program with "make" could be problematic.

Any python 2.7 installation with obspy, matplotlib and plotly 2.7.0 can be
used.

An easy installation procedure is described here:

* Install [Miniconda](https://conda.io/miniconda.html) or the complete [Anaconda](https://www.anaconda.com/download/) that require more disk space.
* Restart your terminal to load the new PATH variables.
* Add the conda-forge channel:  
`conda config --add channels conda-forge`
* Create a virtual environment called "pymaid":  
`conda create -n pymaid python=2.7`
* Activate the environment:  
`source activate pymaid`
* Install obspy:  
`conda install obspy`
* Install plotly 2.7.0:  
`conda install plotly=2.7.0`
* Quit the virtual environment:  
`source deactivate`

In addition to the Python 2.7 installation it is necessary to compile
the wavelet invention program located in `scripts/bin/V103_Sources/` and
`scripts/bin/V103EC_Sources/`. The compiled binaries must be placed in
the "bin" directory and must be named `icdf24_v103_test` and
`icdf24_v103ec_test`.

### 2. USAGE

To use the application: 

* Copy files from server to the "server" directory:  
`scp username@host:\{"*.LOG","*.MER","*.vit"\} server`
* Activate the virtual environment:  
`source activate pymaid`
* Run the main.py file in the "scripts" directory:  
`python scripts/main.py`
* Quit the virtual environment:  
`source deactivate`

The "main.py" file can be edited to select some options:

* A date range between which to process the data can be chosen with
the `begin` and `end` variables. 
* A "redo" flag can be set to True in order to restart the processing
of data for each launch of the script. This flag force the deletion
of the content of the content of the `processed` directory.
* A `events_plotly` flag allow the user to plot interactive figures
of events in a html page. This kind of plot can be disabled to save
disk space.

An additional tool is available to invert a single ".MER" files. For
this, go in the `scripts` directory. Put a Mermaid file with the
extension ".MER" in the `scripts/tool_invert_mer` directory. And
finally run the script: `python tool_invert_mer.py`.
