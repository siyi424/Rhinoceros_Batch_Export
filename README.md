# Rhinoceros_Batch_Export
This project aims at batch exporting some ".stl" files into ".wrl" files with some description infos saved in another ".txt" file. 

## Testing
There are 2 targets.
- [&#10004;] BondingBox (World coordinates)
- [&#10004;] Wsl format


The world coordinate comparison of the manually work and this project: we can see this project gets the right results that we need!
|Object|Manually|Project|IsSame|
|--|--|--|--|
|Bladder_15|min = -17.347,116.193,-755.214<br>max = 24.710,154.025,-732.399<br>dimensions = 42.057, 37.832, 22.815 millimeters|min = -17.347,116.193,-755.214<br>max = 24.710,154.025,-732.399<br>dimensions = 42.057, 37.832, 22.815 millimeters|Yes|
|Brain_18|min = -56.296,133.610,-422.717<br>max = 52.094,260.803,-322.260<br>dimensions = 108.390, 127.193, 100.457 millimeters|min = -56.296,133.610,-422.717<br>max = 52.094,260.803,-322.260<br>dimensions = 108.390, 127.193, 100.457 millimeters|Yes|

To check whether the generated wsl files could be used or not, I utilize the ParaView software to see if I can render them successfully. The answer is YES.

**Right now, I can say this project could do its jobs perfectly ~ Isn't it nice?** :smile:

## Prerequisites
- Rhinoceros software (supporting **PythonScripts**)
- ParaView (or other tools could **render ".wsl" files**)
- Python

## Getting Started
1. **Download "batch_export.py"** at anywhere, just making sure you could find it.
2. At your Workspace folder, **create a new "record.txt" file**. World coordinates data will be saved into this txt file.
3. Open Rhinoceros $\rightarrow$ "Tools" $\rightarrow$ "PythonScripts" $\rightarrow$ Find "batch_export.py" file that you downloaded $\rightarrow$ Choose layers you want to export.
4. Few seconds later, everything will be finished. Just go ahead and see your results!:smile:

## Notes
Commands could be easily replaced if you have other demands ~
