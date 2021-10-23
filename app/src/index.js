const express = require('express');
var cors = require('cors');
const path = require('path');
const handlebars = require('express-handlebars');
const app = express();
const port = process.env.PORT || '9000';

app.use(cors());

const route = require('./routes');
const db = require('./config/db');

db.connect();

app.use(express.urlencoded());
app.use(express.json());

app.engine(
    'hbs',
    handlebars({
        extname: '.hbs',
        helpers: {},
    })
);

app.set('view engine', 'hbs');
app.set('views', path.join(__dirname, 'resources', 'views'));

route(app);

app.listen(port, () => {
    console.log(`App listening at http://localhost:${port}`);
});
