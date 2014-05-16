#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include "r3.h"
#include "r3_str.h"
#include "str_array.h"

#include <sys/time.h>

#define MICRO_IN_SEC 1000000.00
#define SEC_IN_MIN 60
#define NUL  '\0'

long unixtime() {
    struct timeval tp;
    long sec = 0L;
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        return tp.tv_sec;
    }
    return 0;
}

double microtime() {
    struct timeval tp;
    long sec = 0L;
    double msec = 0.0;
    char ret[100];
    
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        msec = (double) (tp.tv_usec / MICRO_IN_SEC);
        sec = tp.tv_sec;
        if (msec >= 1.0)
            msec -= (long) msec;
        return sec + msec;
    }
    return 0;
}


START_TEST (test_ltrim_slash)
{
    fail_if( strcmp( ltrim_slash("/blog") , "blog" ) != 0 );
    fail_if( strcmp( ltrim_slash("blog") , "blog" ) != 0 );
}
END_TEST

START_TEST (test_r3_node_construct_uniq)
{
    node * n = r3_tree_create(10);

    node * child = r3_tree_create(3);

    // fail_if( r3_tree_add_child(n, strdup("/add") , child) != NULL );
    // fail_if( r3_tree_add_child(n, strdup("/add") , child) != NULL );

    r3_tree_free(n);
}
END_TEST

START_TEST (test_r3_node_find_edge)
{
    node * n = r3_tree_create(10);

    node * child = r3_tree_create(3);

    fail_if( r3_tree_add_child(n, strdup("/add") , child) == FALSE );

    fail_if( r3_node_find_edge(n, "/add") == NULL );
    fail_if( r3_node_find_edge(n, "/bar") != NULL );

    r3_tree_free(n);
}
END_TEST


START_TEST (test_compile)
{
    str_array *t;
    node * n;
    n = r3_tree_create(10);


    node *m;
    edge *e ;

    r3_tree_insert_pathn(n, "/zoo", strlen("/zoo"), NULL);
    r3_tree_insert_pathn(n, "/foo", strlen("/foo"), NULL);
    r3_tree_insert_pathn(n, "/bar", strlen("/bar"), NULL);
    r3_tree_compile(n);
    fail_if( n->combined_pattern );
    fail_if( NULL == r3_node_find_edge_str(n, "/", strlen("/") ) );

#ifdef DEBUG
    r3_tree_dump(n, 0);
#endif

    r3_tree_insert_pathn(n, "/foo/{id}", strlen("/foo/{id}"), NULL);
    r3_tree_insert_pathn(n, "/{id}", strlen("/{id}"), NULL);
    r3_tree_compile(n);
    r3_tree_compile(n); // test double compile
#ifdef DEBUG
    r3_tree_dump(n, 0);
#endif
    /*
    fail_if(n->edges[0]->child->combined_pattern == NULL);

    e = r3_node_find_edge_str(n, "/", strlen("/") );
    fail_if( NULL == e );
    */
    /*
    printf( "%s\n", e->pattern );
    printf( "%s\n", e->child->combined_pattern );
    printf( "%s\n", n->edges[0]->child->combined_pattern);
    printf( "%s\n", n->combined_pattern );
    */

    match_entry * entry;

    entry = match_entry_create( "foo" , strlen("/foo") );
    m = r3_tree_match( n , "/foo", strlen("/foo"), entry);
    fail_if( NULL == m );

    entry = match_entry_create( "/zoo" , strlen("/zoo") );
    m = r3_tree_match( n , "/zoo", strlen("/zoo"), entry);
    fail_if( NULL == m );

    entry = match_entry_create( "/bar" , strlen("/bar") );
    m = r3_tree_match( n , "/bar", strlen("/bar"), entry);
    fail_if( NULL == m );

    entry = match_entry_create( "/xxx" , strlen("/xxx") );
    m = r3_tree_match( n , "/xxx", strlen("/xxx"), entry);
    fail_if( NULL == m );

    entry = match_entry_create( "/foo/xxx" , strlen("/foo/xxx") );
    m = r3_tree_match( n , "/foo/xxx", strlen("/foo/xxx"), entry);
    fail_if( NULL == m );

    entry = match_entry_create( "/some_id" , strlen("/some_id") );
    m = r3_tree_match( n , "/some_id", strlen("/some_id"), entry);
    fail_if( NULL == m );
    ck_assert_int_gt( m->endpoint , 0 ); // should not be an endpoint
}
END_TEST

START_TEST (test_compile_slug)
{
    /*
    char * pattern;
    pattern = compile_slug("{id}", strlen("{id}"));
    ck_assert_str_eq( pattern, "([^/]+)" );
    free(pattern);

    pattern = compile_slug("/{id}", strlen("/{id}"));
    ck_assert_str_eq( pattern, "/([^/]+)" );
    free(pattern);

    pattern = compile_slug("-{id}", strlen("-{id}"));
    ck_assert_str_eq( pattern, "-([^-]+)" );
    free(pattern);

    pattern = compile_slug("{id}-{title}", strlen("{id}-{title}"));
    ck_assert_str_eq( pattern, "([^/]+)-([^-]+)" );
    free(pattern);


    pattern = compile_slug("{id:[a-z]+}", strlen("{id:[a-z]+}") );
    ck_assert_str_eq( pattern, "([a-z]+)" );
    free(pattern);


    pattern = compile_slug("/path/{id:[a-z]+}", strlen("/path/{id:[a-z]+}") );
    ck_assert_str_eq( pattern, "/path/([a-z]+)" );
    free(pattern);
    */

    /*
    char * p = malloc(sizeof(char) * 10);
    strncat(p, "foo", 3);
    free(p);
    */





}
END_TEST


START_TEST (test_r3_tree_insert_pathn)
{
    node * n = r3_tree_create(10);

    info("Inserting /foo/bar\n");
    r3_tree_insert_path(n, "/foo/bar", NULL);
    // r3_tree_dump(n, 0);

    info("Inserting /foo/zoo\n");
    r3_tree_insert_path(n, "/foo/zoo", NULL);
    // r3_tree_dump(n, 0);

    info("Inserting /f/id\n");
    r3_tree_insert_path(n, "/f/id" , NULL);
    // r3_tree_dump(n, 0);

    info("Inserting /post/{id}\n");
    r3_tree_insert_pathn(n, "/post/{id}", strlen("/post/{id}"), NULL);
    // r3_tree_dump(n, 0);

    info("Inserting /post/{handle}\n");
    r3_tree_insert_pathn(n, "/post/{handle}", strlen("/post/{handle}"), NULL);
    // r3_tree_dump(n, 0);

    info("Inserting /post/{handle}-{id}\n");
    r3_tree_insert_pathn(n, "/post/{handle}-{id}", strlen("/post/{handle}-{id}"), NULL);
    r3_tree_compile(n);

#ifdef DEBUG
    r3_tree_dump(n, 0);
#endif
    r3_tree_free(n);
}
END_TEST




START_TEST (test_route_split)
{
    str_array *t;

    t = split_route_pattern("/blog", strlen("/blog") );
    fail_if( t == NULL );
    // str_array_dump(t);
    str_array_free(t);

    t = split_route_pattern("/foo/{id}", strlen("/foo/{id}") );
    fail_if( t == NULL );
    // str_array_dump(t);
    fail_if( t->len != 2 );
    str_array_free(t);

    t = split_route_pattern("/foo/bar/{id}", strlen("/foo/bar/{id}") );
    fail_if( t == NULL );
    // str_array_dump(t);
    fail_if( t->len != 3 );
    str_array_free(t);

    t = split_route_pattern("/{title}", strlen("/{title}") );
    fail_if( t == NULL );
    // str_array_dump(t);
    fail_if( t->len != 1 );
    str_array_free(t);

    t = split_route_pattern("/", strlen("/") );
    fail_if( t == NULL );
    // str_array_dump(t);
    fail_if( t->len != 1 );
    str_array_free(t);

}
END_TEST

START_TEST (test_str_array)
{
    str_array * l = str_array_create(3);
    fail_if( l == NULL );

    fail_if( FALSE == str_array_append(l, strdup("abc") ) );
    fail_if( l->len != 1 );

    fail_if( FALSE == str_array_append(l, strdup("foo") ) );
    fail_if( l->len != 2 );

    fail_if( FALSE == str_array_append(l, strdup("bar") ) );
    fail_if( l->len != 3 );

    fail_if( FALSE == str_array_append(l, strdup("zoo") ) );
    fail_if( l->len != 4 );

    fail_if( FALSE == str_array_resize(l, l->cap * 2) );

    str_array_free(l);
}
END_TEST

START_TEST(benchmark_str)
{
    match_entry * entry = calloc( sizeof(entry) , 1 );
    node * n = r3_tree_create(1);

    r3_tree_insert_path(n, "/foo/bar/baz", NULL);
    r3_tree_insert_path(n, "/foo/bar/qux", NULL);
    r3_tree_insert_path(n, "/foo/bar/quux", NULL);
    r3_tree_insert_path(n, "/foo/bar/corge", NULL);
    r3_tree_insert_path(n, "/foo/bar/grault", NULL);
    r3_tree_insert_path(n, "/foo/bar/garply", NULL);
    r3_tree_insert_path(n, "/foo/baz/bar", NULL);
    r3_tree_insert_path(n, "/foo/baz/qux", NULL);
    r3_tree_insert_path(n, "/foo/baz/quux", NULL);
    r3_tree_insert_path(n, "/foo/baz/corge", NULL);
    r3_tree_insert_path(n, "/foo/baz/grault", NULL);
    r3_tree_insert_path(n, "/foo/baz/garply", NULL);
    r3_tree_insert_path(n, "/foo/qux/bar", NULL);
    r3_tree_insert_path(n, "/foo/qux/baz", NULL);
    r3_tree_insert_path(n, "/foo/qux/quux", NULL);
    r3_tree_insert_path(n, "/foo/qux/corge", NULL);
    r3_tree_insert_path(n, "/foo/qux/grault", NULL);
    r3_tree_insert_path(n, "/foo/qux/garply", NULL);
    r3_tree_insert_path(n, "/foo/quux/bar", NULL);
    r3_tree_insert_path(n, "/foo/quux/baz", NULL);
    r3_tree_insert_path(n, "/foo/quux/qux", NULL);
    r3_tree_insert_path(n, "/foo/quux/corge", NULL);
    r3_tree_insert_path(n, "/foo/quux/grault", NULL);
    r3_tree_insert_path(n, "/foo/quux/garply", NULL);
    r3_tree_insert_path(n, "/foo/corge/bar", NULL);
    r3_tree_insert_path(n, "/foo/corge/baz", NULL);
    r3_tree_insert_path(n, "/foo/corge/qux", NULL);
    r3_tree_insert_path(n, "/foo/corge/quux", NULL);
    r3_tree_insert_path(n, "/foo/corge/grault", NULL);
    r3_tree_insert_path(n, "/foo/corge/garply", NULL);
    r3_tree_insert_path(n, "/foo/grault/bar", NULL);
    r3_tree_insert_path(n, "/foo/grault/baz", NULL);
    r3_tree_insert_path(n, "/foo/grault/qux", NULL);
    r3_tree_insert_path(n, "/foo/grault/quux", NULL);
    r3_tree_insert_path(n, "/foo/grault/corge", NULL);
    r3_tree_insert_path(n, "/foo/grault/garply", NULL);
    r3_tree_insert_path(n, "/foo/garply/bar", NULL);
    r3_tree_insert_path(n, "/foo/garply/baz", NULL);
    r3_tree_insert_path(n, "/foo/garply/qux", NULL);
    r3_tree_insert_path(n, "/foo/garply/quux", NULL);
    r3_tree_insert_path(n, "/foo/garply/corge", NULL);
    r3_tree_insert_path(n, "/foo/garply/grault", NULL);
    r3_tree_insert_path(n, "/bar/foo/baz", NULL);
    r3_tree_insert_path(n, "/bar/foo/qux", NULL);
    r3_tree_insert_path(n, "/bar/foo/quux", NULL);
    r3_tree_insert_path(n, "/bar/foo/corge", NULL);
    r3_tree_insert_path(n, "/bar/foo/grault", NULL);
    r3_tree_insert_path(n, "/bar/foo/garply", NULL);
    r3_tree_insert_path(n, "/bar/baz/foo", NULL);
    r3_tree_insert_path(n, "/bar/baz/qux", NULL);
    r3_tree_insert_path(n, "/bar/baz/quux", NULL);
    r3_tree_insert_path(n, "/bar/baz/corge", NULL);
    r3_tree_insert_path(n, "/bar/baz/grault", NULL);
    r3_tree_insert_path(n, "/bar/baz/garply", NULL);
    r3_tree_insert_path(n, "/bar/qux/foo", NULL);
    r3_tree_insert_path(n, "/bar/qux/baz", NULL);
    r3_tree_insert_path(n, "/bar/qux/quux", NULL);
    r3_tree_insert_path(n, "/bar/qux/corge", NULL);
    r3_tree_insert_path(n, "/bar/qux/grault", NULL);
    r3_tree_insert_path(n, "/bar/qux/garply", NULL);
    r3_tree_insert_path(n, "/bar/quux/foo", NULL);
    r3_tree_insert_path(n, "/bar/quux/baz", NULL);
    r3_tree_insert_path(n, "/bar/quux/qux", NULL);
    r3_tree_insert_path(n, "/bar/quux/corge", NULL);
    r3_tree_insert_path(n, "/bar/quux/grault", NULL);
    r3_tree_insert_path(n, "/bar/quux/garply", NULL);
    r3_tree_insert_path(n, "/bar/corge/foo", NULL);
    r3_tree_insert_path(n, "/bar/corge/baz", NULL);
    r3_tree_insert_path(n, "/bar/corge/qux", NULL);
    r3_tree_insert_path(n, "/bar/corge/quux", NULL);
    r3_tree_insert_path(n, "/bar/corge/grault", NULL);
    r3_tree_insert_path(n, "/bar/corge/garply", NULL);
    r3_tree_insert_path(n, "/bar/grault/foo", NULL);
    r3_tree_insert_path(n, "/bar/grault/baz", NULL);
    r3_tree_insert_path(n, "/bar/grault/qux", NULL);
    r3_tree_insert_path(n, "/bar/grault/quux", NULL);
    r3_tree_insert_path(n, "/bar/grault/corge", NULL);
    r3_tree_insert_path(n, "/bar/grault/garply", NULL);
    r3_tree_insert_path(n, "/bar/garply/foo", NULL);
    r3_tree_insert_path(n, "/bar/garply/baz", NULL);
    r3_tree_insert_path(n, "/bar/garply/qux", NULL);
    r3_tree_insert_path(n, "/bar/garply/quux", NULL);
    r3_tree_insert_path(n, "/bar/garply/corge", NULL);
    r3_tree_insert_path(n, "/bar/garply/grault", NULL);
    r3_tree_insert_path(n, "/baz/foo/bar", NULL);
    r3_tree_insert_path(n, "/baz/foo/qux", NULL);
    r3_tree_insert_path(n, "/baz/foo/quux", NULL);
    r3_tree_insert_path(n, "/baz/foo/corge", NULL);
    r3_tree_insert_path(n, "/baz/foo/grault", NULL);
    r3_tree_insert_path(n, "/baz/foo/garply", NULL);
    r3_tree_insert_path(n, "/baz/bar/foo", NULL);
    r3_tree_insert_path(n, "/baz/bar/qux", NULL);
    r3_tree_insert_path(n, "/baz/bar/quux", NULL);
    r3_tree_insert_path(n, "/baz/bar/corge", NULL);
    r3_tree_insert_path(n, "/baz/bar/grault", NULL);
    r3_tree_insert_path(n, "/baz/bar/garply", NULL);
    r3_tree_insert_path(n, "/baz/qux/foo", NULL);
    r3_tree_insert_path(n, "/baz/qux/bar", NULL);
    r3_tree_insert_path(n, "/baz/qux/quux", NULL);
    r3_tree_insert_path(n, "/baz/qux/corge", NULL);
    r3_tree_insert_path(n, "/baz/qux/grault", NULL);
    r3_tree_insert_path(n, "/baz/qux/garply", NULL);
    r3_tree_insert_path(n, "/baz/quux/foo", NULL);
    r3_tree_insert_path(n, "/baz/quux/bar", NULL);
    r3_tree_insert_path(n, "/baz/quux/qux", NULL);
    r3_tree_insert_path(n, "/baz/quux/corge", NULL);
    r3_tree_insert_path(n, "/baz/quux/grault", NULL);
    r3_tree_insert_path(n, "/baz/quux/garply", NULL);
    r3_tree_insert_path(n, "/baz/corge/foo", NULL);
    r3_tree_insert_path(n, "/baz/corge/bar", NULL);
    r3_tree_insert_path(n, "/baz/corge/qux", NULL);
    r3_tree_insert_path(n, "/baz/corge/quux", NULL);
    r3_tree_insert_path(n, "/baz/corge/grault", NULL);
    r3_tree_insert_path(n, "/baz/corge/garply", NULL);
    r3_tree_insert_path(n, "/baz/grault/foo", NULL);
    r3_tree_insert_path(n, "/baz/grault/bar", NULL);
    r3_tree_insert_path(n, "/baz/grault/qux", NULL);
    r3_tree_insert_path(n, "/baz/grault/quux", NULL);
    r3_tree_insert_path(n, "/baz/grault/corge", NULL);
    r3_tree_insert_path(n, "/baz/grault/garply", NULL);
    r3_tree_insert_path(n, "/baz/garply/foo", NULL);
    r3_tree_insert_path(n, "/baz/garply/bar", NULL);
    r3_tree_insert_path(n, "/baz/garply/qux", NULL);
    r3_tree_insert_path(n, "/baz/garply/quux", NULL);
    r3_tree_insert_path(n, "/baz/garply/corge", NULL);
    r3_tree_insert_path(n, "/baz/garply/grault", NULL);
    r3_tree_insert_path(n, "/qux/foo/bar", NULL);
    r3_tree_insert_path(n, "/qux/foo/baz", NULL);
    r3_tree_insert_path(n, "/qux/foo/quux", NULL);
    r3_tree_insert_path(n, "/qux/foo/corge", NULL);
    r3_tree_insert_path(n, "/qux/foo/grault", NULL);
    r3_tree_insert_path(n, "/qux/foo/garply", NULL);
    r3_tree_insert_path(n, "/qux/bar/foo", NULL);
    r3_tree_insert_path(n, "/qux/bar/baz", NULL);
    r3_tree_insert_path(n, "/qux/bar/quux", NULL);
    r3_tree_insert_path(n, "/qux/bar/corge",  (void*) 999);
    r3_tree_insert_path(n, "/qux/bar/grault", NULL);
    r3_tree_insert_path(n, "/qux/bar/garply", NULL);
    r3_tree_insert_path(n, "/qux/baz/foo", NULL);
    r3_tree_insert_path(n, "/qux/baz/bar", NULL);
    r3_tree_insert_path(n, "/qux/baz/quux", NULL);
    r3_tree_insert_path(n, "/qux/baz/corge", NULL);
    r3_tree_insert_path(n, "/qux/baz/grault", NULL);
    r3_tree_insert_path(n, "/qux/baz/garply", NULL);
    r3_tree_insert_path(n, "/qux/quux/foo", NULL);
    r3_tree_insert_path(n, "/qux/quux/bar", NULL);
    r3_tree_insert_path(n, "/qux/quux/baz", NULL);
    r3_tree_insert_path(n, "/qux/quux/corge", NULL);
    r3_tree_insert_path(n, "/qux/quux/grault", NULL);
    r3_tree_insert_path(n, "/qux/quux/garply", NULL);
    r3_tree_insert_path(n, "/qux/corge/foo", NULL);
    r3_tree_insert_path(n, "/qux/corge/bar", NULL);
    r3_tree_insert_path(n, "/qux/corge/baz", NULL);
    r3_tree_insert_path(n, "/qux/corge/quux", NULL);
    r3_tree_insert_path(n, "/qux/corge/grault", NULL);
    r3_tree_insert_path(n, "/qux/corge/garply", NULL);
    r3_tree_insert_path(n, "/qux/grault/foo", NULL);
    r3_tree_insert_path(n, "/qux/grault/bar", NULL);
    r3_tree_insert_path(n, "/qux/grault/baz", NULL);
    r3_tree_insert_path(n, "/qux/grault/quux", NULL);
    r3_tree_insert_path(n, "/qux/grault/corge", NULL);
    r3_tree_insert_path(n, "/qux/grault/garply", NULL);
    r3_tree_insert_path(n, "/qux/garply/foo", NULL);
    r3_tree_insert_path(n, "/qux/garply/bar", NULL);
    r3_tree_insert_path(n, "/qux/garply/baz", NULL);
    r3_tree_insert_path(n, "/qux/garply/quux", NULL);
    r3_tree_insert_path(n, "/qux/garply/corge", NULL);
    r3_tree_insert_path(n, "/qux/garply/grault", NULL);
    r3_tree_insert_path(n, "/quux/foo/bar", NULL);
    r3_tree_insert_path(n, "/quux/foo/baz", NULL);
    r3_tree_insert_path(n, "/quux/foo/qux", NULL);
    r3_tree_insert_path(n, "/quux/foo/corge", NULL);
    r3_tree_insert_path(n, "/quux/foo/grault", NULL);
    r3_tree_insert_path(n, "/quux/foo/garply", NULL);
    r3_tree_insert_path(n, "/quux/bar/foo", NULL);
    r3_tree_insert_path(n, "/quux/bar/baz", NULL);
    r3_tree_insert_path(n, "/quux/bar/qux", NULL);
    r3_tree_insert_path(n, "/quux/bar/corge", NULL);
    r3_tree_insert_path(n, "/quux/bar/grault", NULL);
    r3_tree_insert_path(n, "/quux/bar/garply", NULL);
    r3_tree_insert_path(n, "/quux/baz/foo", NULL);
    r3_tree_insert_path(n, "/quux/baz/bar", NULL);
    r3_tree_insert_path(n, "/quux/baz/qux", NULL);
    r3_tree_insert_path(n, "/quux/baz/corge", NULL);
    r3_tree_insert_path(n, "/quux/baz/grault", NULL);
    r3_tree_insert_path(n, "/quux/baz/garply", NULL);
    r3_tree_insert_path(n, "/quux/qux/foo", NULL);
    r3_tree_insert_path(n, "/quux/qux/bar", NULL);
    r3_tree_insert_path(n, "/quux/qux/baz", NULL);
    r3_tree_insert_path(n, "/quux/qux/corge", NULL);
    r3_tree_insert_path(n, "/quux/qux/grault", NULL);
    r3_tree_insert_path(n, "/quux/qux/garply", NULL);
    r3_tree_insert_path(n, "/quux/corge/foo", NULL);
    r3_tree_insert_path(n, "/quux/corge/bar", NULL);
    r3_tree_insert_path(n, "/quux/corge/baz", NULL);
    r3_tree_insert_path(n, "/quux/corge/qux", NULL);
    r3_tree_insert_path(n, "/quux/corge/grault", NULL);
    r3_tree_insert_path(n, "/quux/corge/garply", NULL);
    r3_tree_insert_path(n, "/quux/grault/foo", NULL);
    r3_tree_insert_path(n, "/quux/grault/bar", NULL);
    r3_tree_insert_path(n, "/quux/grault/baz", NULL);
    r3_tree_insert_path(n, "/quux/grault/qux", NULL);
    r3_tree_insert_path(n, "/quux/grault/corge", NULL);
    r3_tree_insert_path(n, "/quux/grault/garply", NULL);
    r3_tree_insert_path(n, "/quux/garply/foo", NULL);
    r3_tree_insert_path(n, "/quux/garply/bar", NULL);
    r3_tree_insert_path(n, "/quux/garply/baz", NULL);
    r3_tree_insert_path(n, "/quux/garply/qux", NULL);
    r3_tree_insert_path(n, "/quux/garply/corge", NULL);
    r3_tree_insert_path(n, "/quux/garply/grault", NULL);
    r3_tree_insert_path(n, "/corge/foo/bar", NULL);
    r3_tree_insert_path(n, "/corge/foo/baz", NULL);
    r3_tree_insert_path(n, "/corge/foo/qux", NULL);
    r3_tree_insert_path(n, "/corge/foo/quux", NULL);
    r3_tree_insert_path(n, "/corge/foo/grault", NULL);
    r3_tree_insert_path(n, "/corge/foo/garply", NULL);
    r3_tree_insert_path(n, "/corge/bar/foo", NULL);
    r3_tree_insert_path(n, "/corge/bar/baz", NULL);
    r3_tree_insert_path(n, "/corge/bar/qux", NULL);
    r3_tree_insert_path(n, "/corge/bar/quux", NULL);
    r3_tree_insert_path(n, "/corge/bar/grault", NULL);
    r3_tree_insert_path(n, "/corge/bar/garply", NULL);
    r3_tree_insert_path(n, "/corge/baz/foo", NULL);
    r3_tree_insert_path(n, "/corge/baz/bar", NULL);
    r3_tree_insert_path(n, "/corge/baz/qux", NULL);
    r3_tree_insert_path(n, "/corge/baz/quux", NULL);
    r3_tree_insert_path(n, "/corge/baz/grault", NULL);
    r3_tree_insert_path(n, "/corge/baz/garply", NULL);
    r3_tree_insert_path(n, "/corge/qux/foo", NULL);
    r3_tree_insert_path(n, "/corge/qux/bar", NULL);
    r3_tree_insert_path(n, "/corge/qux/baz", NULL);
    r3_tree_insert_path(n, "/corge/qux/quux", NULL);
    r3_tree_insert_path(n, "/corge/qux/grault", NULL);
    r3_tree_insert_path(n, "/corge/qux/garply", NULL);
    r3_tree_insert_path(n, "/corge/quux/foo", NULL);
    r3_tree_insert_path(n, "/corge/quux/bar", NULL);
    r3_tree_insert_path(n, "/corge/quux/baz", NULL);
    r3_tree_insert_path(n, "/corge/quux/qux", NULL);
    r3_tree_insert_path(n, "/corge/quux/grault", NULL);
    r3_tree_insert_path(n, "/corge/quux/garply", NULL);
    r3_tree_insert_path(n, "/corge/grault/foo", NULL);
    r3_tree_insert_path(n, "/corge/grault/bar", NULL);
    r3_tree_insert_path(n, "/corge/grault/baz", NULL);
    r3_tree_insert_path(n, "/corge/grault/qux", NULL);
    r3_tree_insert_path(n, "/corge/grault/quux", NULL);
    r3_tree_insert_path(n, "/corge/grault/garply", NULL);
    r3_tree_insert_path(n, "/corge/garply/foo", NULL);
    r3_tree_insert_path(n, "/corge/garply/bar", NULL);
    r3_tree_insert_path(n, "/corge/garply/baz", NULL);
    r3_tree_insert_path(n, "/corge/garply/qux", NULL);
    r3_tree_insert_path(n, "/corge/garply/quux", NULL);
    r3_tree_insert_path(n, "/corge/garply/grault", NULL);
    r3_tree_insert_path(n, "/grault/foo/bar", NULL);
    r3_tree_insert_path(n, "/grault/foo/baz", NULL);
    r3_tree_insert_path(n, "/grault/foo/qux", NULL);
    r3_tree_insert_path(n, "/grault/foo/quux", NULL);
    r3_tree_insert_path(n, "/grault/foo/corge", NULL);
    r3_tree_insert_path(n, "/grault/foo/garply", NULL);
    r3_tree_insert_path(n, "/grault/bar/foo", NULL);
    r3_tree_insert_path(n, "/grault/bar/baz", NULL);
    r3_tree_insert_path(n, "/grault/bar/qux", NULL);
    r3_tree_insert_path(n, "/grault/bar/quux", NULL);
    r3_tree_insert_path(n, "/grault/bar/corge", NULL);
    r3_tree_insert_path(n, "/grault/bar/garply", NULL);
    r3_tree_insert_path(n, "/grault/baz/foo", NULL);
    r3_tree_insert_path(n, "/grault/baz/bar", NULL);
    r3_tree_insert_path(n, "/grault/baz/qux", NULL);
    r3_tree_insert_path(n, "/grault/baz/quux", NULL);
    r3_tree_insert_path(n, "/grault/baz/corge", NULL);
    r3_tree_insert_path(n, "/grault/baz/garply", NULL);
    r3_tree_insert_path(n, "/grault/qux/foo", NULL);
    r3_tree_insert_path(n, "/grault/qux/bar", NULL);
    r3_tree_insert_path(n, "/grault/qux/baz", NULL);
    r3_tree_insert_path(n, "/grault/qux/quux", NULL);
    r3_tree_insert_path(n, "/grault/qux/corge", NULL);
    r3_tree_insert_path(n, "/grault/qux/garply", NULL);
    r3_tree_insert_path(n, "/grault/quux/foo", NULL);
    r3_tree_insert_path(n, "/grault/quux/bar", NULL);
    r3_tree_insert_path(n, "/grault/quux/baz", NULL);
    r3_tree_insert_path(n, "/grault/quux/qux", NULL);
    r3_tree_insert_path(n, "/grault/quux/corge", NULL);
    r3_tree_insert_path(n, "/grault/quux/garply", NULL);
    r3_tree_insert_path(n, "/grault/corge/foo", NULL);
    r3_tree_insert_path(n, "/grault/corge/bar", NULL);
    r3_tree_insert_path(n, "/grault/corge/baz", NULL);
    r3_tree_insert_path(n, "/grault/corge/qux", NULL);
    r3_tree_insert_path(n, "/grault/corge/quux", NULL);
    r3_tree_insert_path(n, "/grault/corge/garply", NULL);
    r3_tree_insert_path(n, "/grault/garply/foo", NULL);
    r3_tree_insert_path(n, "/grault/garply/bar", NULL);
    r3_tree_insert_path(n, "/grault/garply/baz", NULL);
    r3_tree_insert_path(n, "/grault/garply/qux", NULL);
    r3_tree_insert_path(n, "/grault/garply/quux", NULL);
    r3_tree_insert_path(n, "/grault/garply/corge", NULL);
    r3_tree_insert_path(n, "/garply/foo/bar", NULL);
    r3_tree_insert_path(n, "/garply/foo/baz", NULL);
    r3_tree_insert_path(n, "/garply/foo/qux", NULL);
    r3_tree_insert_path(n, "/garply/foo/quux", NULL);
    r3_tree_insert_path(n, "/garply/foo/corge", NULL);
    r3_tree_insert_path(n, "/garply/foo/grault", NULL);
    r3_tree_insert_path(n, "/garply/bar/foo", NULL);
    r3_tree_insert_path(n, "/garply/bar/baz", NULL);
    r3_tree_insert_path(n, "/garply/bar/qux", NULL);
    r3_tree_insert_path(n, "/garply/bar/quux", NULL);
    r3_tree_insert_path(n, "/garply/bar/corge", NULL);
    r3_tree_insert_path(n, "/garply/bar/grault", NULL);
    r3_tree_insert_path(n, "/garply/baz/foo", NULL);
    r3_tree_insert_path(n, "/garply/baz/bar", NULL);
    r3_tree_insert_path(n, "/garply/baz/qux", NULL);
    r3_tree_insert_path(n, "/garply/baz/quux", NULL);
    r3_tree_insert_path(n, "/garply/baz/corge", NULL);
    r3_tree_insert_path(n, "/garply/baz/grault", NULL);
    r3_tree_insert_path(n, "/garply/qux/foo", NULL);
    r3_tree_insert_path(n, "/garply/qux/bar", NULL);
    r3_tree_insert_path(n, "/garply/qux/baz", NULL);
    r3_tree_insert_path(n, "/garply/qux/quux", NULL);
    r3_tree_insert_path(n, "/garply/qux/corge", NULL);
    r3_tree_insert_path(n, "/garply/qux/grault", NULL);
    r3_tree_insert_path(n, "/garply/quux/foo", NULL);
    r3_tree_insert_path(n, "/garply/quux/bar", NULL);
    r3_tree_insert_path(n, "/garply/quux/baz", NULL);
    r3_tree_insert_path(n, "/garply/quux/qux", NULL);
    r3_tree_insert_path(n, "/garply/quux/corge", NULL);
    r3_tree_insert_path(n, "/garply/quux/grault", NULL);
    r3_tree_insert_path(n, "/garply/corge/foo", NULL);
    r3_tree_insert_path(n, "/garply/corge/bar", NULL);
    r3_tree_insert_path(n, "/garply/corge/baz", NULL);
    r3_tree_insert_path(n, "/garply/corge/qux", NULL);
    r3_tree_insert_path(n, "/garply/corge/quux", NULL);
    r3_tree_insert_path(n, "/garply/corge/grault", NULL);
    r3_tree_insert_path(n, "/garply/grault/foo", NULL);
    r3_tree_insert_path(n, "/garply/grault/bar", NULL);
    r3_tree_insert_path(n, "/garply/grault/baz", NULL);
    r3_tree_insert_path(n, "/garply/grault/qux", NULL);
    r3_tree_insert_path(n, "/garply/grault/quux", NULL);
    r3_tree_insert_path(n, "/garply/grault/corge", NULL);

    r3_tree_compile(n);
    // r3_tree_dump(n, 0);
    // match_entry *entry = calloc( sizeof(entry) , 1 );

    node *m;
    m = r3_tree_match(n , "/qux/bar/corge", strlen("/qux/bar/corge"), NULL);
    fail_if( m == NULL );
    // r3_tree_dump( m, 0 );
    ck_assert_int_eq( (int) m->route_ptr, 999 );


    printf("Benchmarking...\n");
    double s = microtime();
    long N = 5000000;
    for (int i = 0; i < 5000000 ; i++ ) {
        r3_tree_match(n , "/qux/bar/corge", strlen("/qux/bar/corge"), NULL);
    }
    double e = microtime();

    printf("%ld iterations ", N);
    printf("finished in %lf seconds\n", e - s );
    printf("%.2f i/sec\n", N / (e - s) );

    FILE *fp = fopen("bench_str.csv", "a+");
    fprintf(fp, "%ld,%.2f\n", unixtime(), N / (e - s));
    fclose(fp);

}
END_TEST

Suite* r3_suite (void) {
        Suite *suite = suite_create("blah");

        TCase *tcase = tcase_create("testcase");
        tcase_add_test(tcase, test_route_split);
        tcase_add_test(tcase, test_str_array);
        tcase_add_test(tcase, test_ltrim_slash);
        tcase_add_test(tcase, test_r3_node_construct_uniq);
        tcase_add_test(tcase, test_r3_node_find_edge);
        tcase_add_test(tcase, test_r3_tree_insert_pathn);
        tcase_add_test(tcase, test_compile_slug);
        tcase_add_test(tcase, test_compile);

        tcase_add_test(tcase, benchmark_str);

        suite_add_tcase(suite, tcase);

        return suite;
}

int main (int argc, char *argv[]) {
        int number_failed;
        Suite *suite = r3_suite();
        SRunner *runner = srunner_create(suite);
        srunner_run_all(runner, CK_NORMAL);
        number_failed = srunner_ntests_failed(runner);
        srunner_free(runner);
        return number_failed;
}
