module.exports = {
    multipleMongooseToObject: (objects) => objects.map((item) => item.toObject()),
    mongooseToObject: (object) => object.toObject(),
  };