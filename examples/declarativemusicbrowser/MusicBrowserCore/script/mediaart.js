.pragma library

var homePath = "file:///home/user";
var illegalCharacters = /(\(.*\)|\{.*\}|\[.*\]|<.*>|[\(\)_\{\}\[\]\!@#$^&*+=|\/"'?~`])/g
var whitespace = /\s+/g

function getAlbumArtUrl(artist, title) {
    if (artist == "") artist = " ";
    if (title == "") title = " ";

    return homePath
            + "/.cache/media-art/album-"
            + Qt.md5(artist.toLowerCase().replace(illegalCharacters, "").replace(whitespace, " "))
            + "-"
            + Qt.md5(title.toLowerCase().replace(illegalCharacters, "").replace(whitespace, " "))
            + ".jpeg";
}

function formatDuration(duration)
{
    return Math.floor(duration / 60) + ":" + (duration % 60)
}
