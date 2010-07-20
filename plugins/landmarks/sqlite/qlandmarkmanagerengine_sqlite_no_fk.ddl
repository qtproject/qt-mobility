
CREATE TABLE IF NOT EXISTS landmark (
    id INTEGER PRIMARY KEY,
    name TEXT,
    description TEXT,
    icon_url TEXT,
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
    post_office_box TEXT,
    phone TEXT,
    url TEXT
);

CREATE TABLE IF NOT EXISTS landmark_attribute (
    landmark_id INTEGER,
    key TEXT,
    value TEXT,
    PRIMARY KEY (landmark_id,key)
);

CREATE INDEX IF NOT EXISTS landmark_attribute_fk_index ON landmark_attribute(landmark_id);

CREATE TABLE IF NOT EXISTS category (
    id INTEGER PRIMARY KEY,
    name TEXT,
    description TEXT,
    icon_url TEXT
);

CREATE TABLE IF NOT EXISTS category_attribute (
    category_id INTEGER,
    key TEXT,
    value TEXT,
    PRIMARY KEY (category_id,key)
);

CREATE INDEX IF NOT EXISTS category_attribute_fk_index ON category_attribute(category_id);

CREATE TABLE IF NOT EXISTS landmark_category (
    landmark_id INTEGER,
    category_id INTEGER,
    PRIMARY KEY (landmark_id,category_id)
);

CREATE INDEX IF NOT EXISTS landmark_category_fk_lm_index ON landmark_category(landmark_id);
CREATE INDEX IF NOT EXISTS landmark_category_fk_cat_index ON landmark_category(category_id);

