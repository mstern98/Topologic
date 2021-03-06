// SPDX-License-Identifier: MIT WITH bison-exception
// Copyright © 2020 Matthew Stern, Benjamin Michalowicz

#include "../include/topologic.h"

struct fireable *create_fireable(struct graph *graph, struct vertex *vertex, struct vertex_result *args, enum STATES color, int iloop) 
{
    topologic_debug("%s;graph %p;vertex %p;vertex_results %p;color %d;iloop %d", "create_fireable", graph, vertex, args, color, iloop);
    struct fireable *fireable = (struct fireable *)malloc(sizeof(struct fireable));
    if (!fireable)
    {
        topologic_debug("%s;%s;%p", "create_fireable", "could not create fireable", (void *) NULL);
        return NULL;
    }
    fireable->args = (struct vertex_result *)malloc(sizeof(struct vertex_result));
    if (!fireable->args)
    {
        topologic_debug("%s;%s;%p", "create_fireable", "failed to malloc args", (void *) NULL);
        goto free_fireable;
    }
    fireable->args->vertex_argv = NULL;
    if (args->vertex_argv && args->vertex_size > 0)
    {
        fireable->args->vertex_argv = malloc(sizeof(args->vertex_size));
        if (!fireable->args->vertex_argv)
        {
            topologic_debug("%s;%s;%p", "create_fireable", "failed to malloc edge_args", (void *) NULL);
            goto free_args;
        }
        memcpy(fireable->args->vertex_argv, args->vertex_argv, args->vertex_size);
    }
    fireable->args->edge_argv = NULL;
    if (args->edge_argv && args->edge_size > 0)
    {
        fireable->args->edge_argv = malloc(sizeof(args->edge_size));
        if (!fireable->args->vertex_argv)
        {
            topologic_debug("%s;%s;%p", "create_fireable", "failed to malloc vertex_args", (void *) NULL);
            goto free_vertex_argv;
        }
        memcpy(fireable->args->edge_argv, args->edge_argv, args->edge_size);
    }

    fireable->args->vertex_size = args->vertex_size;
    fireable->args->edge_size = args->edge_size;
    fireable->graph = graph;
    fireable->vertex = vertex;
    fireable->color = color;
    fireable->iloop = iloop;
   
    topologic_debug("%s;%s;%p", "create_fireable", "success", fireable);
    return fireable;

free_vertex_argv:
    free(fireable->args->vertex_argv);
    fireable->args->vertex_argv = NULL;
free_args:
    free(fireable->args);
    fireable->args = NULL;
free_fireable:
    free(fireable);
    fireable = NULL;
    return NULL;
}

int destroy_fireable(struct fireable *fireable)
{
    topologic_debug("%s;fireable %p", "destroy_fireable", fireable);
    if (!fireable) {
        topologic_debug("%s;failed;%d", "destroy_fireable", -1);
        return -1;
    }
    fireable->args = NULL;
    fireable->graph = NULL;
    fireable->vertex = NULL;
    fireable->color = PRINT;
    fireable->iloop = 0;
    free(fireable);
    topologic_debug("%s;success;%d", "destroy_fireable", 0);
    return 0;
}
