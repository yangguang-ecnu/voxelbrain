# MRI postprocessing for brain scans #

To study brain based on MRI scans one has to classify voxels into brain/nonbrain tissue. Some studies require thousands of scans, so a range of automatic algorithms were developed for fast classification. The automatic algorithms do not always work properly, so this tool is focused on fast interactive retouching of such automatically processed MRI scans.


# Current Approach #

The editing happens in 2 stages:
  * Interactve detection of anomalies [AnomalyDetection](AnomalyDetection.md)
  * Fast semi-automatic removal of the anomalies [AnomalyRemoval](AnomalyRemoval.md)

# Current Problems #

  * Propagation:
    * Make it more customizable, refactor.
    * Limit propagation to a color range
  * Features:
    * Interface (WxWidgets]

# Software installation #
[Build](Build.md)




