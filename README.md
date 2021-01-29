HOWTO Build:
------------

mkdir build
cd build
cmake ../
make

Data Collection:
------------
The map and trajectory data are publicly available from the OpenStreetMap project. 

The current data used for analysis and research was downloaded by regions from Geofabrik’s website.
The downloaded data was then processed with the publicly available tool from the OsmGraphCreater Github repository: https://github.com/fmi-alg/OsmGraphCreator, where the data can be processed and converted to a text file containing vertices coordinates and edges on the graph.

Available matching algorithms in this tool:
------------
Discrete Fréchet Matching Path (DFMP)
<br />Shortest Discrete Fréchet Matching Path (SDFMP)
<br />Weak Discrete Fréchet Matching Path (WDFMP)
<br />Weak Shortest Discrete Fréchet Matching Path (WDFMP)
<br />Hidden Markov Model (HMM)

User-defined parameters for HMM:
------------
• M – Candidate searching radius<br />
• N – Candidate set size<br />
• 𝜎 – Capture the GPS noise<br />
• α – A value between 0 and 1 and controls the tradeoff between transition and observation weights. Higher α values put more weight on route plausibility.<br />
• R – The maximal ratio between observation distance and candidate distance
that can be considered plausible.\n\n


Command line arguements:
------------
A user needs to run the program twice to complete a map matching task:
The first run is for preprocessing the graph, it requirs the following arguments, and this will output the graph in a text file format that needs to be used for the next run:
<br />Task: “pre-processing”
<br />Input graph
<br />Subsampling threshold
<br /><br />For the second run, the following command-line arguments need to be passed to the program:

Task: “map-matching”
<br />• Subsampled graph
<br />• Input trajectory (either in binary file format or text file format)
<br />• Pre-processing parameters - grid cell size
<br />• Desired map matching solution
<br />• 𝜎 (only for HMM) or “none”
<br />• 𝛼 (only for HMM) or “none”
<br />• N (only for HMM) or “none”
<br />• M (only for HMM) or “none”
<br />• Statistics: “runtime statistics”, or “runtime quality”, or “runtime statistics and quality”, or “none
<br />• Output format: txtOSMID, txtVID, JSON


_txtOSMID: returns one line of vertex OSMIDs per trajectory._

_txtVID: returns one line of vertex IDs per trajectory._

_JSON: For each trajectory, it returns one line of vertex OSMIDs and one line of vertex IDs, as well as the corresponding statistics queried in the statistics argument._


