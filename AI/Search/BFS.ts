type Graph = Record<string, Record<string, number>>;

export function bfs(
  graph: Graph,
  start: string,
  end_group: string[]
): {
  expansion_order: string[];
  goal_path: string[];
  path_cost: number;
} {
  // Boundary Condition
  if (!start || !end_group.length) {
    return {
      expansion_order: [],
      goal_path: [],
      path_cost: 0,
    };
  }

  // Initialize
  const queue: string[] = [start];
  const visited = new Set<string>(); // record the node accessed before
  const predecessors: Record<string, [string, number]> = {};

  let arrived = end_group[0];
  const expansion: string[] = [];

  let end_loop = false;
  while (queue.length) {
    const current_node = queue.shift()!;
    expansion.push(current_node);

    for (const neighbor in graph[current_node]) {
      const neighbor_node_cost = graph[current_node][neighbor];
      if (!visited.has(`${neighbor}`)) {
        visited.add(`${neighbor}`);
        queue.push(neighbor);
        predecessors[neighbor] = [current_node, neighbor_node_cost || 0];
        if (end_group.includes(neighbor)) {
          arrived = neighbor;
          expansion.push(arrived);
          end_loop = true;
          break;
        }
      }
    }

    if (end_loop) {
      break;
    }
  }

  // If end node isn't reached, return
  let has_end = false;
  for (const item of end_group) {
    if (item in predecessors) {
      has_end = true;
    }
  }
  if (!has_end) {
    return {
      expansion_order: [],
      goal_path: [],
      path_cost: 0,
    };
  }

  const path = [arrived];
  let total_cost = 0;

  while (path[path.length - 1] !== start) {
    const predecessor = predecessors[path[path.length - 1]];
    path.push(predecessor[0]);
    total_cost = total_cost + predecessor[1];
  }

  return {
    expansion_order: expansion,
    goal_path: path.reverse(),
    path_cost: total_cost,
  };
}

const graph: Graph = {
  S: { A: 3, B: 20, C: 5 },
  A: { A: 100, B: 13 },
  C: { C: 100, B: 14 },
  B: { G1: 30, G2: 26 },
  G1: {},
  G2: {},
};
const graph1: Graph = {
  S: { A: 14, C: 17 },
  A: { A: 17, B: 2, G: 150 },
  C: { B: 4, G: 2 },
  B: { G: 24 },
};

const start_node = 'S';
const end_node = ['G1', 'G2'];
const shortest_path = bfs(graph, start_node, end_node);
console.log(shortest_path);

const start_node1 = 'S';
const end_node1 = ['G'];
const shortest_path1 = bfs(graph1, start_node1, end_node1);
console.log(shortest_path1);
