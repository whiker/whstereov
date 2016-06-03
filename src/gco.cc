#include <iostream>
#include <stdlib.h>
#include "global.h"
#include "mrf.h"
#include "gco_graph.h"
#include "gco.h"
using namespace std;

int*      gco_active_sites;
int*      gco_active_idx;
GcoGraph  gco_graph(SiteMaxNum);

void gco(Cost data_cost_fn)
{
    gcoInit(data_cost_fn);
    gcoExpansion(10);
}

void gcoInit(Cost data_cost_fn)
{
	mrfInitCost(data_cost_fn);
	mrfInitLabel(false);
	mrfInitNeighPart();
	gco_active_sites = new int[SiteNum];
    gco_active_idx = new int[SiteNum];
}

void gcoExpansion(const int nIter)
{
    int64_t new_engry, old_engry;
    int* label_seq = new int[LabelNum];
    old_engry = mrfEngry();
    
    for (int iter = 0; iter < nIter; iter++)
    {
        cout << old_engry << endl;
        gcoRandomLabelSeq(label_seq);
        for (int l = 0; l < LabelNum; l++)
            gcoAlphaExpansion(label_seq[l]);
        
        new_engry = mrfEngry();
        if (new_engry == old_engry)
            break;
        old_engry = new_engry;
    }
	delete[] label_seq;
}

void gcoRandomLabelSeq(int label_seq[])
{
    for (int i = 0; i < LabelNum; i++)
        label_seq[i] = i;
    
    srand(time(NULL));
    for (int i = 0; i < LabelNum; i++)
    {
        int j = i + rand() % (LabelNum-i);
        int tmp = label_seq[i];
        label_seq[i] = label_seq[j];
        label_seq[j] = tmp;
    }
}

bool gcoAlphaExpansion(int alpha)
{
    int n_active_sites = gcoActiveSites(alpha);
    if (n_active_sites == 0)
        return false;
    
    gcoConstructGraph(alpha, n_active_sites);
    
    // changeToAlphaLabel(alpha, n_active_sites);
}

int gcoActiveSites(int alpha)
{
    int n = 0;
    for (int i = 0; i < SiteNum; i++)
    {
        if (site_label[i] != alpha)
        {
            gco_active_sites[n] = i;
            gco_active_idx[i] = n;
            n++;
        }
        else
            gco_active_idx[i] = -1;
    }
    return n;
}

void gcoConstructGraph(int alpha, int n_active_sites)
{
    int p, fp, q, fq, qi;
    int B, C, D;
    
    gco_graph.setNodeNum(n_active_sites);
    
    // alpha点
    for (int i = 0; i < SiteNum; i++)
    {
        if (site_label[i] != alpha)
            continue;
        for (int j = site_neigh[i][0]; j > 0; j--)
        {
            p = site_neigh[i][j];
            fp = site_label[p];
            if (fp != alpha)
                gco_graph.tlink(gco_active_idx[p], SmoothCost(fp, alpha), 0);
        }
    }
    
    // 非alpha点
    for (int pi = 0; pi < n_active_sites; pi++)
    {
        p = gco_active_sites[pi];
        fp = site_label[p];
        gco_graph.tlink(pi, data_cost[p][fp], data_cost[p][alpha]);
        C = SmoothCost(fp, alpha);
        
        for (int j = site_neigh[p][0]; j > 0; j--)
        {
            q = site_neigh[p][j];
            fq = site_label[q];
            qi = gco_active_idx[q];
            
            if (fq == alpha)
            {
                gco_graph.tlink(pi, C, 0);
            }
            else if (fq == fp)
            {
                gco_graph.nlink(pi, qi, C, C);
            }
            else
            {
                B = SmoothCost(fq, alpha);
                D = SmoothCost(fq, fp);
                if (D > C)
                {
                    gco_graph.tlink(pi, C, 0);
                    gco_graph.tlink(qi, D-C, 0);
                    gco_graph.nlink(pi, qi, B+C-D, 0);
                }
                else
                {
                    gco_graph.tlink(pi, D, 0);
                    gco_graph.nlink(pi, qi, B, C-D);
                }
            }
        }
    }
}

void changeToAlphaLabel(int alpha, int n_active_sites)
{
    int p;
    
    for (int pi = 0; pi < n_active_sites; pi++)
    {
        p = gco_active_sites[pi];
        site_label[p] = alpha;
    }
}