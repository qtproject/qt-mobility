
CREATE TABLE landmark(
    id INTEGER PRIMARY KEY,
    name TEXT,
    description TEXT,
    iconUrl TEXT,
    latitude REAL,
    longitude REAL,
    altitude REAL,
    radius REAL,
    top_left_lat REAL,
    top_left_lon REAL,
    bottom_right_lat REAL,
    bottom_right_lon REAL,
    country TEXT,
    country_code TEXT,
    state TEXT,
    county TEXT,
    district TEXT,
    city TEXT,
    thoroughfare_name TEXT,
    thoroughfare_number TEXT,
    postcode TEXT,
    post_office_box TEXT
);

CREATE TABLE landmark_attribute(
    landmark_id INTEGER,
    key TEXT,
    value TEXT,
    PRIMARY_KEY(landmark_id, key)
);

CREATE INDEX landmark_attribute_fk_index ON landmark_attribute(landmark_id);

CREATE TABLE category(
    id INTEGER PRIMARY KEY,
    name TEXT,
    description TEXT,
    iconUrl TEXT
);

CREATE TABLE category_attribute(
    cat_id INTEGER,
    key TEXT,
    value TEXT,
    PRIMARY_KEY(category_id, key)
);

CREATE INDEX category_attribute_fk_index ON category_attribute(category_id);

CREATE TABLE landmark_category(
    landmark_id INTEGER,
    category_id INTEGER,
    PRIMARY KEY(landmark_id, category_id)
);

CREATE INDEX landmark_category_fk_lm_index ON landmark_category(landmark_id);
CREATE INDEX landmark_category_fk_cat_index ON landmark_category(category_id);

