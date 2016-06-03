#ifndef STEREOV_GCO_H
#define STEREOV_GCO_H

/*
please refer to following papers:

[1] Fast approximate energy minimization via graph cuts.
	Boykov Y, Veksler O, Zabih R. Pattern Analysis and Machine Intelligence, 2001.

[2] What Energy Functions can be Minimized via Graph Cuts?
	V. Kolmogorov, R.Zabih. IEEE TPAMI, 26(2):147-159, Feb 2004. 
*/

void gco(Cost data_cost_fn);

// private
void gcoInit(Cost data_cost_fn);

void gcoExpansion(const int nIter);
void gcoRandomLabelSeq(int label_seq[LabelNum]);
bool gcoAlphaExpansion(int alpha);
int  gcoActiveSites(int alpha);
void gcoConstructGraph(int alpha, int n_active_sites);
void changeToAlphaLabel(int alpha, int n_active_sites);

#endif