/* global vis */

var changeChosenLabelColor = function(ctx, values) {
  values.color = '#ff0000'
}

var changeChosenLabelSize = function(ctx, values) {
  values.size += 1
}

var changeChosenLabelFace = function(ctx, values) {
  values.face = 'serif'
}

var changeChosenLabelMod = function(ctx, values) {
  values.mod = 'bold italic'
}

var changeChosenLabelStrokeWidth = function(ctx, values) {
  values.strokeWidth = 5
}

var changeChosenLabelStrokeColor = function(ctx, values) {
  values.strokeColor = '#00ff00'
}

var changeChosenNodeColor = function(values) {
  values.color = '#ffdd88'
}

var changeChosenNodeBorderWidth = function(values) {
  values.borderWidth = 3
}

var changeChosenNodeBorderColor = function(values) {
  values.borderColor = '#ff0000'
}

var changeChosenNodeSize = function(values) {
  values.size = 30
}

var changeChosenNodeBorderDashes = function(values) {
  values.borderDashes = [10, 10]
}

var changeChosenNodeBorderRadius = function(values) {
  values.borderRadius = 15
}

var changeChosenNodeShadow = function(values) {
  values.shadow = true
}

var changeChosenNodeShadowColor = function(values) {
  values.shadowColor = '#ff0000'
}

var changeChosenNodeShadowSize = function(values) {
  values.shadowSize = 30
}

var changeChosenNodeShadowX = function(values) {
  values.shadowX = -5
}

var changeChosenNodeShadowY = function(values) {
  values.shadowY = -5
}

var changeChosenEdgeToArrow = function(values) {
  values.toArrow = true
}

var changeChosenEdgeToArrowScale = function(values) {
  values.toArrowScale = 2
}

var changeChosenEdgeToArrowType = function(values) {
  values.toArrowType = 'circle'
}

var changeChosenEdgeMiddleArrow = function(values) {
  values.middleArrow = true
}

var changeChosenEdgeMiddleArrowScale = function(values) {
  values.middleArrowScale = 2
}

var changeChosenEdgeMiddleArrowType = function(values) {
  values.middleArrowType = 'circle'
}

var changeChosenEdgeFromArrow = function(values) {
  values.fromArrow = true
}

var changeChosenEdgeFromArrowScale = function(values) {
  values.fromArrowScale = 2
}

var changeChosenEdgeFromArrowType = function(values) {
  values.fromArrowType = 'circle'
}

var changeChosenEdgeArrowStrikethrough = function(values) {
  values.arrowStrikethrough = false
}

var changeChosenEdgeColor = function(values) {
  values.color = '#00ff00'
}

var changeChosenEdgeInheritsColor = function(values) {
  values.inheritsColor = 'both'
}

var changeChosenEdgeOpacity = function(values) {
  values.opacity = 0.25
}

var changeChosenEdgeHidden = function(values) {
  values.hidden = true
}

var changeChosenEdgeShadow = function(values) {
  values.shadow = true
}

var changeChosenEdgeShadowColor = function(values) {
  values.shadowColor = '#00ffff'
}

var changeChosenEdgeShadowSize = function(values) {
  values.shadowSize = 20
}

var changeChosenEdgeShadowX = function(values) {
  values.shadowX = -5
}

var changeChosenEdgeShadowY = function(values) {
  values.shadowY = -5
}

var changeChosenEdgeWidth = function(values) {
  values.width = 5
}

var changeChosenEdgeDashes = function(values) {
  values.dashes = [10, 10]
}

var nodes = [
  {
    id: 1000,
    label: 'label does not change',
    x: -400,
    y: -300,
    chosen: { label: false, node: false }
  },
  {
    id: 1010,
    label: 'label default settings',
    x: -400,
    y: -225,
    chosen: { label: true, node: false }
  },
  {
    id: 1020,
    label: 'label changes color',
    x: -400,
    y: -150,
    chosen: { label: changeChosenLabelColor, node: false }
  },
  {
    id: 1030,
    label: 'label changes size',
    x: -400,
    y: -75,
    chosen: { label: changeChosenLabelSize, node: false }
  },
  {
    id: 1040,
    label: 'label changes face',
    x: -400,
    y: 0,
    chosen: { label: changeChosenLabelFace, node: false }
  },
  {
    id: 1050,
    label: 'label changes modifier',
    x: -400,
    y: 75,
    chosen: { label: changeChosenLabelMod, node: false }
  },
  {
    id: 1060,
    label: 'label changes stokeWidth',
    x: -400,
    y: 150,
    chosen: { label: changeChosenLabelStrokeWidth, node: false }
  },
  {
    id: 1070,
    label: 'label changes stokeColor',
    x: -400,
    y: 225,
    font: { strokeWidth: 2 },
    chosen: { label: changeChosenLabelStrokeColor, node: false }
  },

  {
    id: 2000,
    label: 'node does not change',
    x: 0,
    y: -300,
    chosen: { label: false, node: false }
  },
  {
    id: 2010,
    label: 'node default settings',
    x: 0,
    y: -225,
    chosen: { label: false, node: true }
  },
  {
    id: 2020,
    label: 'node changes color',
    x: 0,
    y: -150,
    chosen: { label: false, node: changeChosenNodeColor }
  },
  {
    id: 2030,
    label: 'node changes borderWidth',
    x: 0,
    y: -75,
    chosen: { label: false, node: changeChosenNodeBorderWidth }
  },
  {
    id: 2040,
    label: 'node changes borderColor',
    x: 0,
    y: 0,
    chosen: { label: false, node: changeChosenNodeBorderColor }
  },
  {
    id: 2050,
    label: 'shaped nodes change size',
    x: 0,
    y: 70,
    shape: 'star',
    chosen: { label: false, node: changeChosenNodeSize }
  },
  {
    id: 2051,
    x: -60,
    y: 70,
    shape: 'dot',
    chosen: { label: false, node: changeChosenNodeSize }
  },
  {
    id: 2052,
    x: -120,
    y: 70,
    shape: 'diamond',
    chosen: { label: false, node: changeChosenNodeSize }
  },
  {
    id: 2053,
    x: 60,
    y: 70,
    shape: 'square',
    chosen: { label: false, node: changeChosenNodeSize }
  },
  {
    id: 2054,
    x: 120,
    y: 70,
    shape: 'triangle',
    chosen: { label: false, node: changeChosenNodeSize }
  },
  {
    id: 2055,
    x: 165,
    y: 70,
    shape: 'triangleDown',
    chosen: { label: false, node: changeChosenNodeSize }
  },
  {
    id: 2060,
    label: 'node changes borderDashes',
    x: 0,
    y: 150,
    chosen: { label: false, node: changeChosenNodeBorderDashes }
  },
  {
    id: 2070,
    label: 'node changes borderRadius',
    x: 0,
    y: 225,
    chosen: { label: false, node: changeChosenNodeBorderRadius }
  },
  {
    id: 2080,
    label: 'node changes shadow',
    x: 0,
    y: 300,
    chosen: { label: false, node: changeChosenNodeShadow }
  },
  {
    id: 2090,
    label: 'node changes shadowColor',
    x: 0,
    y: 375,
    shadow: true,
    chosen: { label: false, node: changeChosenNodeShadowColor }
  },
  {
    id: 2100,
    label: 'node changes shadowSize',
    x: 0,
    y: 450,
    shadow: true,
    chosen: { label: false, node: changeChosenNodeShadowSize }
  },
  {
    id: 2110,
    label: 'node changes shadowX',
    x: 0,
    y: 525,
    shadow: true,
    chosen: { label: false, node: changeChosenNodeShadowX }
  },
  {
    id: 2120,
    label: 'node changes shadowY',
    x: 0,
    y: 600,
    shadow: true,
    chosen: { label: false, node: changeChosenNodeShadowY }
  },

  {
    id: 3000,
    x: 275,
    y: -310,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3001,
    x: 525,
    y: -210,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3010,
    x: 275,
    y: -235,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3011,
    x: 525,
    y: -135,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3020,
    x: 275,
    y: -160,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3021,
    x: 525,
    y: -60,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3030,
    x: 275,
    y: -85,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3031,
    x: 525,
    y: 15,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3040,
    x: 275,
    y: -10,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3041,
    x: 525,
    y: 90,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3050,
    x: 275,
    y: 65,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3051,
    x: 525,
    y: 165,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3060,
    x: 275,
    y: 140,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3061,
    x: 525,
    y: 240,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3070,
    x: 275,
    y: 215,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3071,
    x: 525,
    y: 315,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3080,
    x: 275,
    y: 290,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3081,
    x: 525,
    y: 390,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3090,
    x: 275,
    y: 365,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3091,
    x: 525,
    y: 465,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3100,
    x: 275,
    y: 440,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3101,
    x: 525,
    y: 540,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3110,
    x: 275,
    y: 515,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3111,
    x: 525,
    y: 615,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3120,
    x: 575,
    y: -310,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3121,
    x: 825,
    y: -210,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3130,
    x: 575,
    y: -235,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3131,
    x: 825,
    y: -135,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3140,
    x: 575,
    y: -160,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3141,
    x: 825,
    y: -60,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3150,
    x: 575,
    y: -85,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3151,
    x: 825,
    y: 15,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3160,
    x: 575,
    y: -10,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3161,
    x: 825,
    y: 90,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3170,
    x: 575,
    y: 65,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3171,
    x: 825,
    y: 165,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3180,
    x: 575,
    y: 140,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3181,
    x: 825,
    y: 240,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3190,
    x: 575,
    y: 215,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3191,
    x: 825,
    y: 315,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3200,
    x: 575,
    y: 290,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3201,
    x: 825,
    y: 390,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3210,
    x: 575,
    y: 365,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3211,
    x: 825,
    y: 465,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  },
  {
    id: 3220,
    x: 575,
    y: 440,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#dd6644',
    borderWidth: 0
  },
  {
    id: 3221,
    x: 825,
    y: 540,
    chosen: false,
    shape: 'dot',
    size: 10,
    color: '#6699dd',
    borderWidth: 0
  }
]

var edges = [
  {
    id: 4000,
    from: 3000,
    to: 3001,
    label: 'edge does not change',
    chosen: false
  },
  {
    id: 4010,
    from: 3010,
    to: 3011,
    label: 'edge has default settings',
    chosen: { label: false, edge: true }
  },
  {
    id: 4020,
    from: 3020,
    to: 3021,
    label: 'edge changes toArrow',
    chosen: { label: false, edge: changeChosenEdgeToArrow }
  },
  {
    id: 4030,
    from: 3030,
    to: 3031,
    label: 'edge changes toArrowScale',
    arrows: { to: true },
    chosen: { label: false, edge: changeChosenEdgeToArrowScale }
  },
  {
    id: 4040,
    from: 3040,
    to: 3041,
    label: 'edge changes toArrowType',
    arrows: { to: true },
    chosen: { label: false, edge: changeChosenEdgeToArrowType }
  },
  {
    id: 4050,
    from: 3050,
    to: 3051,
    label: 'edge changes middleArrow',
    chosen: { label: false, edge: changeChosenEdgeMiddleArrow }
  },
  {
    id: 4060,
    from: 3060,
    to: 3061,
    label: 'edge changes middleArrowScale',
    arrows: { middle: true },
    chosen: { label: false, edge: changeChosenEdgeMiddleArrowScale }
  },
  {
    id: 4070,
    from: 3070,
    to: 3071,
    label: 'edge changes middleArrowType',
    arrows: { middle: true },
    chosen: { label: false, edge: changeChosenEdgeMiddleArrowType }
  },
  {
    id: 4080,
    from: 3080,
    to: 3081,
    label: 'edge changes fromArrow',
    chosen: { label: false, edge: changeChosenEdgeFromArrow }
  },
  {
    id: 4090,
    from: 3090,
    to: 3091,
    label: 'edge changes fromArrowScale',
    arrows: { from: true },
    chosen: { label: false, edge: changeChosenEdgeFromArrowScale }
  },
  {
    id: 4100,
    from: 3100,
    to: 3101,
    label: 'edge changes fromArrowType',
    arrows: { from: true },
    chosen: { label: false, edge: changeChosenEdgeFromArrowType }
  },
  {
    id: 4110,
    from: 3110,
    to: 3111,
    label: 'edge changes arrowStrikethrough',
    arrows: { to: true, from: true },
    width: 7,
    chosen: { label: false, edge: changeChosenEdgeArrowStrikethrough }
  },
  {
    id: 4120,
    from: 3120,
    to: 3121,
    label: 'edge changes color',
    chosen: { label: false, edge: changeChosenEdgeColor }
  },
  {
    id: 4130,
    from: 3130,
    to: 3131,
    label: 'edge inherits color',
    chosen: { label: false, edge: changeChosenEdgeInheritsColor }
  },
  {
    id: 4140,
    from: 3140,
    to: 3141,
    label: 'edge changes opacity',
    chosen: { label: false, edge: changeChosenEdgeOpacity }
  },
  {
    id: 4150,
    from: 3150,
    to: 3151,
    label: 'edge changes hidden',
    chosen: { label: false, edge: changeChosenEdgeHidden }
  },
  {
    id: 4160,
    from: 3160,
    to: 3161,
    label: 'edge changes shadow',
    width: 7,
    chosen: { label: false, edge: changeChosenEdgeShadow }
  },
  {
    id: 4170,
    from: 3170,
    to: 3171,
    label: 'edge changes shadowColor',
    shadow: true,
    width: 7,
    chosen: { label: false, edge: changeChosenEdgeShadowColor }
  },
  {
    id: 4180,
    from: 3180,
    to: 3181,
    label: 'edge changes shadowSize',
    shadow: true,
    width: 7,
    chosen: { label: false, edge: changeChosenEdgeShadowSize }
  },
  {
    id: 4190,
    from: 3190,
    to: 3191,
    label: 'edge changes shadowX',
    shadow: true,
    width: 7,
    chosen: { label: false, edge: changeChosenEdgeShadowX }
  },
  {
    id: 4200,
    from: 3200,
    to: 3201,
    label: 'edge changes shadowY',
    shadow: true,
    width: 7,
    chosen: { label: false, edge: changeChosenEdgeShadowY }
  },
  {
    id: 4210,
    from: 3210,
    to: 3211,
    label: 'edge changes width',
    chosen: { label: false, edge: changeChosenEdgeWidth }
  },
  {
    id: 4220,
    from: 3220,
    to: 3221,
    label: 'edge changes dashes',
    chosen: { label: false, edge: changeChosenEdgeDashes }
  }
]

var container = document.getElementById('mynetwork')
var data = {
  nodes: nodes,
  edges: edges
}
var options = {
  edges: {
    font: {
      size: 16
    },
    selfReferenceSize: 30
  },
  nodes: {
    shape: 'box',
    widthConstraint: 250,
    color: '#cccccc',
    margin: 10,
    font: {
      size: 16
    }
  },
  physics: {
    enabled: false
  },
  interaction: {
    hover: true
  }
}
// network
new vis.Network(container, data, options)
