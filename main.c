//Ofure Ukpebor
/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define RAD 6

typedef struct Edge{//structure to represent edges
    int src;
    int dst;
    int weight;
} Edge;

typedef struct Graph{ //structure to represent graph
    int vertices;
    int edges;
    Edge* edge;
} Graph;

typedef struct Subset{ //structure to represent subsets for Kruskal
    int parent;
    int rank;
} Subset;

Graph* createGraph (int V, int E){
    Graph* graph = (Graph*) malloc( sizeof(Graph));
    graph->vertices = V;
    graph->edges = E;
    graph->edge = (Edge*) malloc( graph->edges * sizeof(Edge));

    return graph;
}

int findComponent(Subset subsets[], int n){
    if ( subsets[n].parent != n ){
        subsets[n].parent = findComponent(subsets, subsets[n].parent);
    }
    return subsets[n].parent;
}

void joinComponent (Subset subsets[], int i, int j){//subset function for join component
    int i_root = findComponent(subsets, i);
    int j_root = findComponent(subsets, j);
    
    if (subsets[i_root].rank < subsets[j_root].rank){
        subsets[i_root].parent = j_root;}
    else if (subsets[i_root].rank > subsets[j_root].rank){
        subsets[j_root].parent = i_root;}
    else{
        subsets[j_root].parent = i_root;
        subsets[i_root].rank++;
    }
}

long int nChooser (int n, int r){//n choose r possible combination of edges
    if ( (n==r) || (r==0) || (n==0) ){
        return 1;
    }
    else{ 
        return ( nChooser ((n-1), r ) + nChooser( (n-1), (r-1) ));
    }
}

int cmpfunc ( const void * i, const void * j ){ //compare function for quicksort
    return ( ((int*)i)[2] - ((int*)j)[2] );
}

void MST(int n, int p[][2], int e[][2]){
    int i, j;
    int ncr = nChooser(n, 2); //30 choose 2 combination of edges

    int graph[ncr][3]; //create graph of all vertex-edges combination
    int weight;
    
    int c = 0;
    for (i=0; i<n; i++){
        for (j=i; j<n; j++){
            if (i != j){
                weight = sqrt((pow((p[i][0]-p[j][0]), 2)+pow((p[i][1]-p[j][1]), 2)));//euclidean distance 
                graph[c][0] = i;
                graph[c][1] = j;
                graph[c][2] = weight;
                c++;
            }
        }
    }
    
    qsort(graph, ncr, sizeof(graph[0]), cmpfunc);//quicksort the edges of the graph
    
    Graph* kruskalGraph = createGraph(n, ncr); //create kruskal graph
    Edge edges[n];
    
    for (i=0; i<ncr; i++){
        kruskalGraph->edge[i].src = graph[i][0];
        kruskalGraph->edge[i].dst = graph[i][1];
    }
    
    Subset* subsets = (Subset*) malloc(ncr * sizeof(Subset));
    
    for (j=0; j<n; j++){
        subsets[j].parent = j;
        subsets[j].rank = 0;
    }
    
    int count = 0;
    int sortedEdge = 0;
    
    while (count < n-1){//kruskal graph is one edge less than full graph
        Edge nextEdge = kruskalGraph->edge[sortedEdge++];
        int i = findComponent(subsets, nextEdge.src);
        int j = findComponent(subsets, nextEdge.dst);
        
        if (i != j){
            edges[count++] = nextEdge;
            joinComponent(subsets, i, j);
        }
    }
    
     // for ( i=0; i<counter; i++ ){
     // printf("[%d] [%d] weight: %d\n", resultEdges[i].src, resultEdges[i].dst, resultEdges[i].weight);}

    for (i=0; i<count; i++){//put the edges of the kruskalGraph in the edges array
        e[i][0] = edges[i].src;
        e[i][1] = edges[i].dst;
    }
    
}


Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Homework 3: Minimum Spanning Tree";
char *icon_name_string = "HW3: MST";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;


int main(int argc, char **argv)
{ int points[1000][2];
    int edges[1000][2];
    int number_of_points;
    FILE *fp;
    if( argc < 2)
    {  printf(" needs input file name as command line argument\n");
        return -1;
    }
    if( (fp = fopen( argv[1], "r"))==NULL )
    {  printf(" could not open input file %s\n", argv[1]);
        return -1;
    }
    number_of_points =0;
    while( ( fscanf(fp, "%d %d\n",
                    &(points[number_of_points][0]),
                    &(points[number_of_points][1]) ) ==2 )
          && (number_of_points < 999 ) )
    {  number_of_points +=1;}
    if( number_of_points == 0 )
    {  printf(" did not find any points in file\n", argv[1]);
        return -1;
    }
    else if (number_of_points >= 999 )
    {  printf(" too many points in file\n", argv[1]);
        return -1;
    }
    else
        printf(" found %d points\n", number_of_points);
    /* opening display: basic connection to X Server */
    if( (display_ptr = XOpenDisplay(display_name)) == NULL )
    { printf("Could not open display. \n"); return -1;}
    printf("Connected to X server  %s\n", XDisplayName(display_name) );
    screen_num = DefaultScreen( display_ptr );
    screen_ptr = DefaultScreenOfDisplay( display_ptr );
    color_map  = XDefaultColormap( display_ptr, screen_num );
    display_width  = DisplayWidth( display_ptr, screen_num );
    display_height = DisplayHeight( display_ptr, screen_num );
    
    printf("Width %d, Height %d, Screen Number %d\n",
           display_width, display_height, screen_num);
    /* creating the window */
    border_width = 10;
    win_x = 0; win_y = 0;
    win_width = display_width/2;
    win_height = win_width; /*square window*/
    
    win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num),
                             win_x, win_y, win_width, win_height, border_width,
                             BlackPixel(display_ptr, screen_num),
                             WhitePixel(display_ptr, screen_num) );
    /* now try to put it on screen, this needs cooperation of window manager */
    size_hints = XAllocSizeHints();
    wm_hints = XAllocWMHints();
    class_hints = XAllocClassHint();
    if( size_hints == NULL || wm_hints == NULL || class_hints == NULL )
    { printf("Error allocating memory for hints. \n"); return -1;}
    
    size_hints -> flags = PPosition | PSize | PMinSize  ;
    size_hints -> min_width = 60;
    size_hints -> min_height = 60;
    
    XStringListToTextProperty( &win_name_string,1,&win_name);
    XStringListToTextProperty( &icon_name_string,1,&icon_name);
    
    wm_hints -> flags = StateHint | InputHint ;
    wm_hints -> initial_state = NormalState;
    wm_hints -> input = False;
    
    class_hints -> res_name = "x_use_example";
    class_hints -> res_class = "examples";
    
    XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,
                     size_hints, wm_hints, class_hints );
    
    /* what events do we want to receive */
    XSelectInput( display_ptr, win,
                 ExposureMask | StructureNotifyMask | ButtonPressMask );
    
    /* finally: put window on screen */
    XMapWindow( display_ptr, win );
    
    XFlush(display_ptr);
    
    /* create graphics context, so that we may draw in this window */
    gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
    XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
    XSetLineAttributes( display_ptr, gc, 4, LineSolid, CapRound, JoinRound);
    /* and three other graphics contexts, to draw in yellow and red and grey*/
    gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
    XSetLineAttributes(display_ptr, gc_yellow, 6, LineSolid,CapRound, JoinRound);
    if( XAllocNamedColor( display_ptr, color_map, "yellow",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color yellow\n"); return -1;}
    else
        XSetForeground( display_ptr, gc_yellow, tmp_color1.pixel );
    gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
    XSetLineAttributes( display_ptr, gc_red, 4, LineSolid, CapRound, JoinRound);
    if( XAllocNamedColor( display_ptr, color_map, "red",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color red\n"); return -1;}
    else
        XSetForeground( display_ptr, gc_red, tmp_color1.pixel );
    gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
    if( XAllocNamedColor( display_ptr, color_map, "light grey",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color grey\n"); return -1;}
    else
        XSetForeground( display_ptr, gc_grey, tmp_color1.pixel );
    
    /* and now it starts: the event loop */
    while(1)
    {  int i;
        int MST_computed = 0;
        XNextEvent( display_ptr, &report );
        switch( report.type )
        {
            case Expose:
                /* (re-)draw the figure. This event happens
                 each time some part ofthe window gets exposed (becomes visible) */
                if( MST_computed )
                {  /* if the MST is known, draw the edges first */
                    for( i=0; i < number_of_points-1; i++ )
                    {
                        XDrawLine(display_ptr, win, gc_red,
                                  (points[edges[i][0]][0]* win_width)/500,
                                  (points[edges[i][0]][1]* win_height)/500,
                                  (points[edges[i][1]][0]* win_width)/500,
                                  (points[edges[i][1]][1]* win_height)/500 );
                    }
                }
                /* now draw the points */
                for( i=0; i < number_of_points; i++ )
                {
                    XFillArc(display_ptr, win, gc,
                             (points[i][0]* win_width)/500 - RAD,
                             (points[i][1]* win_height)/500 - RAD,
                             2*RAD, 2*RAD, 0, 360*64 );
                }
                break; /* end of redrawing the figure */
            case ConfigureNotify:
                /* This event happens when the user changes the size of the window*/
                win_width = report.xconfigure.width;
                win_height = report.xconfigure.height;
                break;
            case ButtonPress:
                /* This event happens when the user pushes a mouse button. */
                /* clear MST */
                for( i = 0; i< number_of_points -1; i++ )
                { edges[i][0] = edges[i][1] = 0;
                }
                MST_computed = 0;
                /* compute MST */
                printf("calling MST with %d points\n", number_of_points);
                fflush(stdout);
                MST( number_of_points, points, edges );
                printf("finished MST\n"); fflush(stdout);
                MST_computed = 1;
                /* draw MST */
                for( i=0; i < number_of_points-1; i++ )
                {
                    XDrawLine(display_ptr, win, gc_red,
                              (points[edges[i][0]][0]* win_width)/500,
                              (points[edges[i][0]][1]* win_height)/500,
                              (points[edges[i][1]][0]* win_width)/500,
                              (points[edges[i][1]][1]* win_height)/500 );
                }
                for( i=0; i < number_of_points; i++ )
                {
                    XFillArc(display_ptr, win, gc,
                             (points[i][0]* win_width)/500 - RAD,
                             (points[i][1]* win_height)/500 -RAD,
                             2*RAD, 2*RAD, 0, 360*64 );
                }
                break;
            default:
                /* this is a catch-all for other events; it does not do anything.
                 One could look at the report type to see what the event was */
                break;
        }
        
    }
    return 0;
}