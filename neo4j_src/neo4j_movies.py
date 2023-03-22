"""
A demonstration of local clustering on a neo4j database.
"""
import argparse
import os
import sys

import stag.neo4j
import stag.cluster


def parse_args():
    parser = argparse.ArgumentParser(description="Movies Demonstration")
    parser.add_argument('--uri', type=str,
                        help="The location of the Neo4j database."
                             "Overrides the NEO4J_URI environment variable.")
    parser.add_argument('--user', type=str,
                        help="The username to log into the database."
                             "Overrides the NEO4J_USER environment variable.")
    parser.add_argument('--password', type=str,
                        help="The password to access the database."
                             "Overrides the NEO4J_PASSWORD environment variable.")
    parser.add_argument('--volume',
                        type=int,
                        help="The target volume for the returned cluster. Default: 100.",
                        default=100)
    parser.add_argument('movie', type=str, help="The movie to search for.")
    return parser.parse_args()


def main():
    args = parse_args()

    user = args.user
    if user is None or user == '':
        try:
            user = os.environ['NEO4J_USER']
        except KeyError:
            print("Couldn't find neo4j user. Try setting the NEO4J_USER environment"
                  " variable.")
            return 1

    uri = args.uri
    if uri is None:
        try:
            uri = os.environ['NEO4J_URI']
        except KeyError:
            print("Couldn't find neo4j uri. Try setting the NEO4J_URI environment"
                  " variable.")
            return 1

    password = args.password
    if password is None:
        try:
            password = os.environ['NEO4J_PASSWORD']
        except KeyError:
            print("Couldn't find neo4j password. Try setting the NEO4J_PASSWORD environment"
                  " variable.")
            return 1

    # Create the graph object
    g = stag.neo4j.Neo4jGraph(uri, user, password)

    # Find the node id corresponding to the matrix
    node_id = g.query_id('title', args.movie)
    if node_id is None:
        print(f"Couldn't find a movie titled {args.movie}.")
        g.__del__()
        return 1

    # Find a local cluster
    cluster = stag.cluster.local_cluster(g, node_id, args.volume)

    # Display the cluster
    print(f"Found cluster with the following {len(cluster)} nodes: ")
    for v_id in cluster:
        labels = g.query_node_labels(v_id)
        if 'Movie' in labels:
            title = g.query_property(v_id, 'title')
            print(f"Movie: {title}")
        elif 'Person' in labels:
            name = g.query_property(v_id, 'name')
            print(f"Person: {name}")

    # Tidy up the graph object to close the connection to the database
    g.__del__()
    return 0


if __name__ == "__main__":
    res = main()
    sys.exit(res)
