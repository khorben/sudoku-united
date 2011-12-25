.pragma library

function formatDuration(ms) {
    var elapsed = ms / 1000
    var minutes = parseInt(elapsed / 60)
    var seconds = parseInt(elapsed % 60)

    if (minutes < 10)
        minutes = "0" + minutes;
    if (seconds < 10)
        seconds = "0" + seconds;

    return minutes + ":" + seconds
}
