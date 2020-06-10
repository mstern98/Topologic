#include "../include/topologic.h"

void print_edges(struct graph *graph, struct AVLTree *edges, const char *indent, FILE *out) {
    struct stack *stack = init_stack();
    struct edge *edge = NULL;
    preorder(edges, stack);
    fprintf(out, "%s\"Edges\": {\n", indent);
    while ((edge = (struct edge *) pop(stack)) != NULL) {
        fprintf(out, "%s\t\"%d\": {\n", indent, edge->id);
        fprintf(out, "%s\t\t\"id\": %d,\n", indent, edge->id);
        fprintf(out, "%s\t\t\"from\": \"%p\",\n", indent, edge->a);
        fprintf(out, "%s\t\t\"to\": \"%p\",\n", indent, edge->b);
        fprintf(out, "%s\t\t\"edge_type\": %d,\n", indent, edge->edge_type);
        fprintf(out, "%s\t\t\"bi_edge\": \"%p\"", indent, edge->bi_edge);
        if ((graph->lvl_verbose & FUNCTIONS) == FUNCTIONS) {
            fprintf(out, ",\n%s\t\t\"f\": \"%p\"", indent, edge->f);
        }
        if ((graph->lvl_verbose & GLOBALS) == GLOBALS) {  
            fprintf(out, ",\n%s\t\t\"glbl\": \"%p\"", indent, edge->glbl);
        }
        fprintf(out, "\n%s\t}", indent);
        if (stack->length > 0)
            fprintf(out, ",\n");
        else fprintf(out, "\n");
    }
    fprintf(out, "%s}", indent);
    destroy_stack(stack);

}

void print_state(struct graph *graph, FILE *out)
{
    /*Called by print and does a pre-order traversal of all the data in each vertex*/
    int vertex_id = 0;
	void *glbl = NULL;
    //int edge_sharedc;
    union shared_edge *edge_shared = NULL;
    struct vertex_result *(*f)(void *) = NULL;
    struct vertex *v = NULL;
    struct stack *stack = init_stack();

    inorder(graph->vertices, stack);
    fprintf(out, "\t\"Vertices\": {\n");
    while ((v = (struct vertex *) pop(stack)) != NULL) {
        vertex_id = v->id;
        f = v->f;
        edge_shared = v->shared;
        glbl = v->glbl;

        //TODO: Setup on Verbose
        if ((graph->lvl_verbose & VERTICES) == VERTICES) {
            fprintf(out, "\t\t\"%d\": {\n", vertex_id);
            fprintf(out, "\t\t\t\"id\": %d,\n", vertex_id);
            fprintf(out, "\t\t\t\"active\": %d", v->is_active);
            if ((graph->lvl_verbose & FUNCTIONS) == FUNCTIONS) {
                fprintf(out, ",\n\t\t\t\"f\": \"%p\"", f);
            }
            if ((graph->lvl_verbose & GLOBALS) == GLOBALS) {
                fprintf(out, ",\n\t\t\t\"glbl\": \"%p\",\n", glbl);
                fprintf(out, "\t\t\t\"edge_shared\": \"%p\"", edge_shared);
            }
            if ((graph->lvl_verbose & EDGES) == EDGES) {
                fprintf(out, ",\n");
                print_edges(graph, v->edge_tree, "\t\t\t", out);
            }
            fprintf(out, "\n\t\t}");
        } else if ((graph->lvl_verbose & EDGES) == EDGES) {
            fprintf(out, "\t\t\"%d\": {\n", vertex_id);
            print_edges(graph, v->edge_tree, "\t\t\t", out);
            fprintf(out, "\n\t\t}");
        }
        if (stack->length > 0) 
            fprintf(out, ",\n");
        else fprintf(out, "\n");
    }
    fprintf(out, "\t}\n");
    destroy_stack(stack);
}

void print(struct graph *graph)
{
    if (!graph)
        return;

    pthread_mutex_lock(&graph->lock);
    if (graph->lvl_verbose == NO_VERB)
    {
        pthread_mutex_unlock(&graph->lock);
        return;
    }

    int dirfd = open("./", O_DIRECTORY | O_RDONLY);
    if (dirfd == -1) return;
    char buffer[256];
    sprintf(buffer, "state_%d.json", graph->state_count);
    int fd = openat(dirfd, buffer, O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU);
    if (fd == -1) {
        close(dirfd);
        pthread_mutex_unlock(&graph->lock);
        return;
    }

    FILE *out = fdopen(fd, "w");
    if (!out) {
        perror("");
        fprintf(stderr, "FAILED OUT\n");
        close(fd);
        close(dirfd);
        pthread_mutex_unlock(&graph->lock);
        return;
    }

    /**TODO: Print enums**/
    fprintf(out, "{\n");
    fprintf(out, " \"graph\": {\n");
    fprintf(out, "\t\"state\": %d,\n\t\"max_state_repeats\": %d,\n\t\"timestamps\": %d,\n\t\"verbosity\": %d,\n\t\"nodes\": %d,\n", graph->state_count,
    graph->max_state_changes, 
    graph->snapshot_timestamp,
    graph->lvl_verbose,
    graph->vertices->size);
    print_state(graph, out);
    fprintf(out, " }\n");
    fprintf(out, "}\n");

    fclose(out);
    close(dirfd);
    pthread_mutex_unlock(&graph->lock);
}
