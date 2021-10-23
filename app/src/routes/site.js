const router = require('express').Router();

const siteController = require('../app/controllers/SiteController');

router.get('/', siteController.index);
router.post('/location', siteController.createLocation);
router.post('/delete-location', siteController.deleteLocation);
router.get('/location', siteController.getLocations);
module.exports = router;
