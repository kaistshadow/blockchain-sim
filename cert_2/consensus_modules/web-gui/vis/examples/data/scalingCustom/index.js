/* global vis */
var nodes = null
var edges = null

/* eslint-disable */
function draw() {
  /* eslint-enable */
  // create people.
  // value corresponds with the age of the person
  nodes = [
    { id: 1, value: 2, label: 'Algie' },
    { id: 2, value: 31, label: 'Alston' },
    { id: 3, value: 12, label: 'Barney' },
    { id: 4, value: 16, label: 'Coley' },
    { id: 5, value: 17, label: 'Grant' },
    { id: 6, value: 15, label: 'Langdon' },
    { id: 7, value: 6, label: 'Lee' },
    { id: 8, value: 5, label: 'Merlin' },
    { id: 9, value: 30, label: 'Mick' },
    { id: 10, value: 18, label: 'Tod' }
  ]

  // create connections between people
  // value corresponds with the amount of contact between two people
  edges = [
    { from: 2, to: 8, value: 3 },
    { from: 2, to: 9, value: 5 },
    { from: 2, to: 10, value: 1 },
    { from: 4, to: 6, value: 8 },
    { from: 5, to: 7, value: 2 },
    { from: 4, to: 5, value: 1 },
    { from: 9, to: 10, value: 2 },
    { from: 2, to: 3, value: 6 },
    { from: 3, to: 9, value: 4 },
    { from: 5, to: 3, value: 1 },
    { from: 2, to: 7, value: 4 }
  ]

  // Instantiate our network object.
  var container = document.getElementById('mynetwork')
  var data = {
    nodes: nodes,
    edges: edges
  }
  var options = {
    nodes: {
      shape: 'dot',
      scaling: {
        customScalingFunction: function(min, max, total, value) {
          return value / total
        },
        min: 5,
        max: 150
      }
    }
  }
  new vis.Network(container, data, options)
}
