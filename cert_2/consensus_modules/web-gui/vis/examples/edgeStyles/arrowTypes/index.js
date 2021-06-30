/* global vis */
var arrow_types = [
  'arrow',
  'bar',
  'circle',
  'box',
  'crow',
  'curve',
  'inv_curve',
  'diamond',
  'triangle',
  'inv_triangle',
  'vee'
]

// update list of arrow types in html body
var nof_types = arrow_types.length
var list_contents = []
for (let i = 0; i < nof_types; i++) {
  list_contents.push("<code>'" + arrow_types[i] + "'</code>")
}
var mylist = document.getElementById('arrow_type_list')
mylist.innerHTML = list_contents.join(', ')

// create an array with nodes
var node_attrs = new Array()
var nodes = arrow_types.slice()
nodes.push('end')
console.log(nodes)
var nof_nodes = nodes.length
for (let i = 0; i < nof_nodes; i++) {
  node_attrs[i] = {
    id: i + 1,
    label: nodes[i]
  }
}

nodes = new vis.DataSet(node_attrs)

// create an array with edges
var edge_attrs = new Array()
var nof_edges = nof_nodes - 1
for (var i = 0; i < nof_edges; i++) {
  edge_attrs[i] = {
    from: i + 1,
    to: i + 2,
    arrows: {
      to: {
        enabled: true,
        type: arrow_types[i]
      }
    }
  }
}

var edges = new vis.DataSet(edge_attrs)

// create a network
var container = document.getElementById('mynetwork')
var data = {
  nodes: nodes,
  edges: edges
}

var options = {
  /*
    // Enable this to make the endpoints smaller/larger
    edges: {
      arrows: {
        to: {
          scaleFactor: 5
        }
      }
    }
*/
}

new vis.Network(container, data, options)
