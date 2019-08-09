var util = require('../../../util')

var Label = require('./shared/Label').default
var ComponentUtil = require('./shared/ComponentUtil').default
var Box = require('./nodes/shapes/Box').default
var Circle = require('./nodes/shapes/Circle').default
var CircularImage = require('./nodes/shapes/CircularImage').default
var Database = require('./nodes/shapes/Database').default
var Diamond = require('./nodes/shapes/Diamond').default
var Dot = require('./nodes/shapes/Dot').default
var Ellipse = require('./nodes/shapes/Ellipse').default
var Icon = require('./nodes/shapes/Icon').default
var Image = require('./nodes/shapes/Image').default
var Square = require('./nodes/shapes/Square').default
var Hexagon = require('./nodes/shapes/Hexagon').default
var Star = require('./nodes/shapes/Star').default
var Text = require('./nodes/shapes/Text').default
var Triangle = require('./nodes/shapes/Triangle').default
var TriangleDown = require('./nodes/shapes/TriangleDown').default
var { printStyle } = require('../../../shared/Validator')

/**
 * A message sent along a visible edge
 */

class Package {
  /**
 * @param {Object} options        values specific to this edge, must contain at least 'from' and 'to'
 * @param {Object} body           shared state from Network instance
 * @param {Object} globalOptions  options from the EdgesHandler instance
 * @param {Object} defaultOptions default options from the EdgeHandler instance. Value and reference are constant
 */
  constructor(options, body, imagelist, globalOptions, defaultOptions) {

    if (body === undefined) {
      throw new Error("No body provided");
    }

    this.options = util.bridgeObject(globalOptions);
    this.globalOptions = globalOptions;
    this.defaultOptions = defaultOptions;
    this.body = body;

    // constants
    this.radiusMin = defaultOptions.radiusMin;
    this.radiusMax = defaultOptions.radiusMax;
    this.imagelist = imagelist;
    this.network = network;

    // initialize variables
    this.id = undefined;
    this.from = undefined;
    this.to = undefined;
    this.title = undefined; //useless, replaced by label in vis
    //this.shape = defaultOptions.shape;
    this.radius = defaultOptions.radius;
    this.color = defaultOptions.color;
    this.image = defaultOptions.image;
    this.value = undefined;
    this.progress =  0.0;
    this.timestamp = undefined;
    this.duration = defaultOptions.duration;
    this.autoProgress = true;
    this.radiusFixed = false;

    this.labelModule = new Label(this.body, this.options, false /* Not edge label */);
    // set properties
    this.setOptions(options);
  }

  /**
   * Set or overwrite options for the package
   * @param {Object} options  an object with options
   * @returns {null|boolean} null if no options, boolean if date changed
   */

  setOptions(options) {
    var currentShape = this.options.shape;
    if (!options) {
      return;
    }

    if (options.id !== undefined) {this.id = options.id;}
    if (options.edge !== undefined) {this.edgeId = options.edge;}
    if (options.value !== undefined) {options.value = parseFloat(options.value);}
    if (options.radius != undefined) {this.radius = options.radius;}
    if (options.image != undefined) {this.image = options.image;}
    if (options.color != undefined) {this.color = options.color;}
    if (options.dashlength != undefined) {this.dashlength = options.dashlength;}
    if (options.dashgap != undefined) {this.dashgap = options.dashgap;}
    if (options.altdashlength != undefined) {this.altdashlength = options.altdashlength;}
    if (options.progress != undefined) {this.progress = options.progress;}
    if (options.duration != undefined) {this.duration = options.duration;}

    this.radiusFixed = this.radiusFixed || (options.radius != undefined);
    this.autoProgress = (this.autoProgress == true) ? (options.progress == undefined) : false;

    if (this.shape == 'image') {
      this.radiusMin = defaultOptions.widthMin;
      this.radiusMax = defaultOptions.widthMax;
    }

    // handle progress
    if (this.progress < 0.0) {this.progress = 0.0;}
    if (this.progress > 1.0) {this.progress = 1.0;}

    // check if edge exists
    this.connect();

    // this transforms all shorthands into fully defined options
    Package.parseOptions(this.options, options, true, this.globalOptions);

    var pile = [options, this.options, this.defaultOptions];
    this.chooser = ComponentUtil.choosify('package', pile);
    
    this._load_images();
    this.updateLabelModule(options);
    this.updateShape(currentShape);

    if (options.hidden !== undefined) {
      dataChanged = true;
    }

  }

  /**
   * Load the images from the options, for the packages that need them.
   *
   * TODO: The imageObj members should be moved to CircularImageBase.
   *       It's the only place where they are required.
   *
   * @private
   */

  _load_images() {
    // Don't bother loading for packages without images
    if (this.options.shape !== 'circularImage' && this.options.shape !== 'image') {
      return;
    }

    if (this.options.image === undefined) {
      throw new Error("Option image must be defined for package type '" + this.options.shape + "'");
    }

    if (this.imagelist === undefined) {
      throw new Error("Internal Error: No images provided");
    }

    if (typeof this.options.image === 'string') {
      this.imageObj = this.imagelist.load(this.options.image, this.options.brokenImage, this.id);
    } else {
      if (this.options.image.unselected === undefined) {
        throw new Error("No unselected image provided");
      }

      this.imageObj = this.imagelist.load(this.options.image.unselected, this.options.brokenImage, this.id);

      if (this.options.image.selected !== undefined) {
        this.imageObjAlt = this.imagelist.load(this.options.image.selected, this.options.brokenImage, this.id);
      } else {
        this.imageObjAlt = undefined;
      }
    }
  }

  /**
   * update the options in the label module
   *
   * @param {Object} options
   */

  updateLabelModule(options) {
    var pile = [options, this.options, this.globalOptions, // Currently set global package options
    this.defaultOptions];

    this.labelModule.update(this.options, pile);

    if (this.labelModule.baseSize !== undefined) {
      this.baseFontSize = this.labelModule.baseSize;
    }
  }

  /**
   *
   * @param {string} currentShape
   */

  updateShape(currentShape) {
    if (currentShape === this.options.shape && this.shape) {
      this.shape.setOptions(this.options, this.imageObj, this.imageObjAlt);
    } else {
      // choose draw method depending on the shape
      switch (this.options.shape) {
        case 'box':
          this.shape = new Box(this.options, this.body, this.labelModule);
          break;
        case 'circle':
          this.shape = new Circle(this.options, this.body, this.labelModule);
          break;
        case 'circularImage':
          this.shape = new CircularImage(this.options, this.body, this.labelModule, this.imageObj, this.imageObjAlt);
          break;
        case 'database':
          this.shape = new Database(this.options, this.body, this.labelModule);
          break;
        case 'diamond':
          this.shape = new Diamond(this.options, this.body, this.labelModule);
          break;
        case 'dot':
          this.shape = new Dot(this.options, this.body, this.labelModule);
          break;
        case 'ellipse':
          this.shape = new Ellipse(this.options, this.body, this.labelModule);
          break;
        case 'icon':
          this.shape = new Icon(this.options, this.body, this.labelModule);
          break;
        case 'image':
          this.shape = new Image(this.options, this.body, this.labelModule, this.imageObj, this.imageObjAlt);
          break;
        case 'square':
          this.shape = new Square(this.options, this.body, this.labelModule);
          break;
        case 'hexagon':
          this.shape = new Hexagon(this.options, this.body, this.labelModule);
          break;
        case 'star':
          this.shape = new Star(this.options, this.body, this.labelModule);
          break;
        case 'text':
          this.shape = new Text(this.options, this.body, this.labelModule);
          break;
        case 'triangle':
          this.shape = new Triangle(this.options, this.body, this.labelModule);
          break;
        case 'triangleDown':
          this.shape = new TriangleDown(this.options, this.body, this.labelModule);
          break;
        default:
          this.shape = new Ellipse(this.options, this.body, this.labelModule);
          break;
      }
    }
    this.needsRefresh();
  }

  /**
   * Connect to the edge and end nodes of the package
   */

  connect() {
    if (!this.body.edges.hasOwnProperty(this.edgeId)) {
      throw "Edge with id " + this.edgeId + " not found";
    }
  }

  /**
   * Reset the calculated size of the package, forces it to recalculate its size
   */

  needsRefresh() {
    this.shape.refreshNeeded = true;
  }

  /**
   * Set a new value for the progress of the package
   * @param {number} progress    A value between 0 and 1
   */
  setProgress(progress) {
    this.progress = progress;
    this.autoProgress = false;
  }

  /**
   * Draw this package in the given canvas
   * The 2d context of a HTML canvas can be retrieved by canvas.getContext("2d");
   * @param {CanvasRenderingContext2D}   ctx
   */
  draw(ctx) {
    let values = this.getFormattingValues()
    let pos = this._getPosition()
    this.shape.draw(ctx, pos.x, pos.y, this.selected, this.hover, values)
  }

  /**
   * Calculate the current position of the package
   * @return {Object} position    The object has parameters x and y.
   */
  _getPosition() {
    let edgeType = this.body.edges[this.edgeId].edgeType
    let viaNode = edgeType.getViaNode()
    let point = edgeType.getPoint(this.progress, viaNode)
    return {
        "x" : point.x,
        "y" : point.y
    }
  }

  /**
   * Get the title of this package.
   * @return {string} title    The title of the package, or undefined when no
   *                           title has been set.
   */
  getTitle() {
    return this.options.title;
  }

  /**
  * Retrieve the value of the package. Can be undefined
  * @return {Number} value
  */
  getValue() {
    return this.options.value;
  }

  /**
   * Update the bounding box of the shape
   * @param {CanvasRenderingContext2D}   ctx
   */
  updateBoundingBox(ctx) {
    this.shape.updateBoundingBox(this.x, this.y, ctx)
  }

  /**
   * Recalculate the size of this package in the given canvas
   * The 2d context of a HTML canvas can be retrieved by canvas.getContext("2d")
   * @param {CanvasRenderingContext2D}   ctx
   */
  resize(ctx) {
    let values = this.getFormattingValues()
    this.shape.resize(ctx, this.selected, this.hover, values)
  }

  /**
   * Adjust the value range of the package. The package will adjust its size
   * based on its value.
   * @param {number} min
   * @param {number} max
   * @param {number} total
   */
  setValueRange(min, max, total) {
    if (this.options.value !== undefined) {
      var scale = this.options.scaling.customScalingFunction(
        min,
        max,
        total,
        this.options.value
      )
      var sizeDiff = this.options.scaling.max - this.options.scaling.min
      if (this.options.scaling.label.enabled === true) {
        var fontDiff =
          this.options.scaling.label.max - this.options.scaling.label.min
        this.options.font.size =
          this.options.scaling.label.min + scale * fontDiff
      }
      this.options.size = this.options.scaling.min + scale * sizeDiff
    } else {
      this.options.size = this.baseSize
      this.options.font.size = this.baseFontSize
    }

    this.updateLabelModule()
  }

  /**
   * This process all possible shorthands in the new options and makes sure that the parentOptions are fully defined.
   * Static so it can also be used by the handler.
   *
   * @param {Object} parentOptions
   * @param {Object} newOptions
   * @param {boolean} [allowDeletion=false]
   * @param {Object} [globalOptions={}]
   * @param {Object} [groupList]
   * @static
   */

  static parseOptions(
    parentOptions,
    newOptions,
    allowDeletion = false,
    globalOptions = {}
  ) {
    var fields = ['color', 'shadow', 'size'];
    util.selectiveNotDeepExtend(fields, parentOptions, newOptions, allowDeletion);

    // merge the shadow options into the parent.
    util.mergeOptions(parentOptions, newOptions, 'shadow', globalOptions);

    // individual shape newOptions
    if (newOptions.color !== undefined && newOptions.color !== null) {
      var parsedColor = util.parseColor(newOptions.color);
      util.fillIfDefined(parentOptions.color, parsedColor);
    } else if (allowDeletion === true && newOptions.color === null) {
      parentOptions.color = util.bridgeObject(globalOptions.color); // set the object back to the global options
    }

    if (allowDeletion === true && newOptions.font === null) {
      parentOptions.font = util.bridgeObject(globalOptions.font); // set the object back to the global options
    }

    // handle the scaling options, specifically the label part
    if (newOptions.scaling !== undefined) {
      util.mergeOptions(parentOptions.scaling, newOptions.scaling, 'label', globalOptions.scaling);
    }
  }

  /**
   *
   * @returns {{color: *, borderWidth: *, borderColor: *, size: *, borderDashes: (boolean|Array|allOptions.nodes.shapeProperties.borderDashes|{boolean, array}), borderRadius: (number|allOptions.nodes.shapeProperties.borderRadius|{number}|Array), shadow: *, shadowColor: *, shadowSize: *, shadowX: *, shadowY: *}}
   */
  getFormattingValues() {
    let values = {
      color: this.options.color.background,
      borderWidth: this.options.borderWidth,
      borderColor: this.options.color.border,
      size: this.options.size,
      borderDashes: this.options.shapeProperties.borderDashes,
      borderRadius: this.options.shapeProperties.borderRadius,
      shadow: this.options.shadow.enabled,
      shadowColor: this.options.shadow.color,
      shadowSize: this.options.shadow.size,
      shadowX: this.options.shadow.x,
      shadowY: this.options.shadow.y
    }
    if (this.selected || this.hover) {
      if (this.chooser === true) {
        if (this.selected) {
          values.borderWidth *= 2
          values.color = this.options.color.highlight.background
          values.borderColor = this.options.color.highlight.border
          values.shadow = this.options.shadow.enabled
        } else if (this.hover) {
          values.color = this.options.color.hover.background
          values.borderColor = this.options.color.hover.border
          values.shadow = this.options.shadow.enabled
        }
      } else if (typeof this.chooser === 'function') {
        this.chooser(values, this.options.id, this.selected, this.hover)
        if (values.shadow === false) {
          if (
            values.shadowColor !== this.options.shadow.color ||
            values.shadowSize !== this.options.shadow.size ||
            values.shadowX !== this.options.shadow.x ||
            values.shadowY !== this.options.shadow.y
          ) {
            values.shadow = true
          }
        }
      }
    } else {
      values.shadow = this.options.shadow.enabled
    }
    return values
  }

}

export default Package
