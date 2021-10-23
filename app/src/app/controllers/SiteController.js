const Location = require('../models/Location');
const { multipleMongooseToObject } = require('../../util/mongoose');

const SiteController = {
    index: async (req, res, next) => {
        try {
            const locations = await Location.find({});
            res.render('home');
        } catch (err) {
            return res.status(500).json({ msg: err.message });
        }
    },
    getLocations: async (req, res, next) => {
        try {
            let locations = await Location.find({});
            return res.json(locations);
        } catch (err) {
            return res.status(500).json({ msg: err.message });
        }
    },
    createLocation: async (req, res, next) => {
        try {
            let location = await Location.findOne({ id: req.body.id });
            if (location === null) {
                location = new Location(req.body);
                await location.save();
            } else {
                location.id = req.body.id;
                location.lat = req.body.lat;
                location.long = req.body.long;
                location.function = req.body.function;
                await location.save();
            }
            return res.json({ msg: 'successfully' });
        } catch (err) {
            return res.status(500).json({ msg: err.message });
        }
    },
    deleteLocation: async (req, res, next) => {
        try {
            await Location.deleteOne({ id: req.body.id }).exec();
            return res.json({ msg: 'successfully' });
        } catch (err) {
            return res.status(500).json({ msg: err.message });
        }
    },
};

module.exports = SiteController;
