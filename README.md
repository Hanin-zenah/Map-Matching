Data Collection:
------------
The map and trajectory data are publicly available from the OpenStreetMap project. 

The current data used for analysis and research was downloaded by regions from Geofabrik’s website： https://www.geofabrik.de/<br />
The downloaded data was then processed with the publicly available tool from the OsmGraphCreater Github repository: https://github.com/fmi-alg/OsmGraphCreator, where the data can be processed and converted to a text file containing vertices coordinates and edges on the graph.

Available matching algorithms in this tool:
------------
Discrete Fréchet Matching Path (DFMP)
<br />Shortest Discrete Fréchet Matching Path (SDFMP)
<br />Weak Discrete Fréchet Matching Path (WDFMP)
<br />Weak Shortest Discrete Fréchet Matching Path (WSDFMP)
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


Building the Tool
------------
1. git clone --recursive https://github.com/MapMatching-research/Map-Matching mapmatching \
cd mapmatching

2. mkdir build
<br />cd build
<br />cmake ../
<br />make

Using the Tool
------------
preprocess --help \
hmm_matching --help \
DFMP_matching --help \
WDFMP_matching --help 

References:
------------
Alt, H., & Godau, M. (1995). Computing The Fréchet Distance Between Two Polygonal Curves. International Journal of Computational Geometry & Applications, 05(01n02), 75-91. doi:10.1142/s0218195995000064
<br />Alt, H., Efrat, A., Rote, G., & Wenk, C. (2003). Matching planar maps. Journal of Algorithms, 49(2), 262-283. doi:10.1016/s0196-6774(03)00085-3
<br />Brakatsoulas, S., Pfoser, D., Salas, R., & Wenk, C. (2005). On Map-matching Vehicle Tracking Data. In Proc. 31st VLDB Conf. 853–864.
<br />E.Thomas & M.Heikki. (1994). Computing Discrete Frechet Distance.
<br />Koller, H., Widhalm, P., Dragaschnig, M., & Graser, A. (2015). Fast Hidden Markov Model Map-Matching for Sparse and Noisy Trajectories. 2015 IEEE 18th International Conference on Intelligent Transportation Systems. doi:10.1109/itsc.2015.411
<br />Newson, P., & Krumm, J. (2009). Hidden Markov map matching through noise and sparseness. Proceedings of the 17th ACM SIGSPATIAL International Conference on Advances in Geographic Information Systems - GIS 09. doi:10.1145/1653771.1653818
<br />Seybold, M. P. (2017). Robust Map Matching for Heterogeneous Data via Dominance Decompositions. Proceedings of the 2017 SIAM International Conference on Data Mining, 813-821. doi:10.1137/1.9781611974973.91
