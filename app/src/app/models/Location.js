const mongoose = require('mongoose');
const Schema = mongoose.Schema;
const ObjectId = Schema.ObjectId;

const Location = new Schema(
    {
        id: { type: String, require: true },
        lat: { type: String, require: true },
        long: { type: String, require: true },
        function: { type: String, require: true },
    },
    {
        timestamps: true,
    }
);

module.exports = mongoose.model('Location', Location);
