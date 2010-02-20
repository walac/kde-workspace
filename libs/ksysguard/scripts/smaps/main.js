var sizeKeys; /* The keys which contain a size - e.g. Size, Pss, Rss etc */
var kernelPageSize;  /* The size of the kernel page size.  -1 if it's not the same for all. */
var mmuPageSize;     /* The size of the mmu page size.  -1 if it's not the same for all. */

function removeItem(items, item) {
    var i = 0;
    while (i < items.length) {
        if (items[i] == item) {
            items.splice(i, 1);
            break;
        } else {
            i++;
        }
    }
    return items;
}

function readSmapsFile() {
    if( !window.process.fileExists("/proc/" + window.process.pid + "/smaps" ) ) {
        if( window.process.fileExists("/proc/" + window.process.pid ) ) {  //Check that it's not just a timing issue - the process might have already ended
            setHtml("<h1>Sorry</h1>Your system is not currently supported (/proc/"+window.process.pid+"/smaps was not found)");
        }
        return;
    }
    var smaps = window.process.readFile("/proc/" + window.process.pid + "/smaps");
    if(!smaps) {
        if( window.process.fileExists("/proc/" + window.process.pid ) ) {  //Check that it's not just a timing issue - the process might have already ended
            setHtml("<h1>Sorry</h1>Could not read detailed memory information (/proc/"+window.process.pid+"/smaps could not be read)");
        }
        return;
    }
    return smaps.split('\n');
}

function parseSmaps() {
    var smaps = readSmapsFile();
    if(!smaps)
        return;
    sizeKeys = new Array();
    kernelPageSize = undefined;
    mmuPageSize = undefined;

    var data = new Array();  /* This is a 0 indexed array */
    /* data contains many dataBlocks */
    var dataBlock; /* This is a hash table */

    var headingRegex = /^([0-9A-Fa-f]+-[0-9A-Fa-f]+) +([^ ]*) +([0-9A-Fa-f]+) +(\d+:\d+) +(\d+) +(.*)$/;
    var lineRegex = /^([^ ]+): +(\d+) kB$/;
    for(var i = 0; i < smaps.length; i++) {
        lineMatch = lineRegex.exec(smaps[i]);
        if(lineMatch) {
            var key = lineMatch[1];
            dataBlock[ key ] = parseInt(lineMatch[2]);  /* E.g.  dataBlock['Size'] = 84 */
            /* Size - Virtual memory space (useless) */
            /* RSS - Includes video card memory etc */
            /* PSS - */
            /* Shared + Private = RSS, but it's more accurate to instead make Shared = Pss - Private */
            if(data.length == 0)
                sizeKeys.push(lineMatch[1]);
        } else if(headingMatch = headingRegex.exec(smaps[i])) {
            if(dataBlock)
                data.push(dataBlock);
            dataBlock = new Array();
            dataBlock['address'] = headingMatch[1]; /* Address in address space in the process that it occupies */
            dataBlock['perms'] = headingMatch[2];   /* Read, Write, eXecute, Shared, Private (copy on write) */
            dataBlock['offset'] = headingMatch[3];  /* Offset into the device */
            dataBlock['dev'] = headingMatch[4];     /* Device (major,minor) */
            dataBlock['inode'] = headingMatch[5];   /* inode on the device - 0 means no inode for the memory region - e.g bss */
            dataBlock['pathname'] = headingMatch[6];
        } else if(smaps[i] != "") {
            throw("Could not parse '" + smaps[i]) + "'";
        }
    }
    if(dataBlock)
        data.push(dataBlock);

    // Add in totals and check page sizes
    for(var i = 0; i < data.length; i++) {
        data[i]['Private'] = data[i]['Private_Clean'] + data[i]['Private_Dirty'];
        data[i]['Shared'] = data[i]['Shared_Clean'] + data[i]['Shared_Dirty'];
        if(!kernelPageSize)
            kernelPageSize = data[i]['KernelPageSize'];
        else if(data[i]['KernelPageSize'] && kernelPageSize != data[i]['KernelPageSize'])
            kernelPageSize = -1;
        if(!mmuPageSize)
            mmuPageSize = data[i]['MMUPageSize'];
        else if(data[i]['mmuPageSize'] && mmuPageSize != data[i]['MMUPageSize'])
            mmuPageSize = -1;

    }
    if(mmuPageSize != -1)
        removeItem(sizeKeys, 'MMUPageSize');
    if(kernelPageSize != -1)
        removeItem(sizeKeys, 'KernelPageSize');
    var sizeKeysIncludingCombined = sizeKeys.concat(['Private', 'Shared']);

    // Now build up another hash table, collapsing the pathname values
    var combinedHash = new Array();
    for(var i = 0; i < data.length; i++) {
        var pathname = data[i]['pathname'];
        if(pathname == "") //Count anonymous mappings (mmap to /dev/zero) as part of the heap
            pathname = "[heap]";
        if(!combinedHash[pathname])
            combinedHash[pathname] = new Array();
        for(var j = 0; j < sizeKeysIncludingCombined.length; j++) {
            var key = sizeKeysIncludingCombined[j]
            if(combinedHash[pathname][key])
                combinedHash[pathname][key] += data[i][key];
            else
                combinedHash[pathname][key] = data[i][key];
        }
    }
    //Convert hash table to an array so that we can sort it
    var combined = new Array();
    var i = 0;
    for(var key in combinedHash) {
        combined[i] = combinedHash[key];
        combined[i]['pathname'] = key;
        i++;
    }i
    return [data,combined];
}
function calculateTotal(data, info) {
    var total = 0;
    for(var i = 0; i < data.length; i++) {
        total += data[i][info];
    }
    return total;
}
function formatKB(kb) {
    var format = "";
    if(kb < 2048) /* less than 2MB, write as just KB */
        format = kb + " KB";
    else if(kb < (1048576)) /* less than 1GB, write in MB */
        format = (kb/1024).toFixed(1) + " MB";
    else
        format = (kb/1048576).toFixed(1) + " GB";
    return "<span class=\"memory\">" + format + "</span>";
}
function sortDataByInfo(data, info) {
    return data.sort( function(a,b) { return b[info] - a[info]; } );
}
function getHtmlTableForLibrarySummary(data, info, total) {
    var sortedData = sortDataByInfo(data, info);
    var html = "";
    var htmlIsForHidddenTBody = false;
    for(var i = 0; i < sortedData.length; i++) {
        var value = sortedData[i][info];
        if(value == 0)
            break; //Do not show libraries with a value of 0 KB
        if( i == 5 && sortedData.length > 8) {
            document.getElementById('tbody' + info).innerHTML = html;
            html = "";
            htmlIsForHiddenTBody = true;
        }
        var pathname = sortedData[i]['pathname'];
        html += "<tr><td class='memory'>" + value + " KB</td><td>" + pathname + "</td></tr>";
    }
    if(htmlIsForHiddenTBody)
        document.getElementById('tbody' + info + 'Hidden').innerHTML = html;
    else
        document.getElementById('tbody' + info).innerHTML = html;
}
function getHtmlSummary(combined) {
    var pss = calculateTotal(combined,'Pss');
    var rss = calculateTotal(combined,'Rss');
    var private_clean = calculateTotal(combined,'Private_Clean');
    var private_dirty = calculateTotal(combined,'Private_Dirty');
    var private_total = private_clean + private_dirty;
    var shared_clean = calculateTotal(combined,'Shared_Clean');
    var shared_dirty = calculateTotal(combined,'Shared_Dirty');
    var shared_total = shared_clean + shared_dirty;
    var swap = calculateTotal(combined,'Swap');
    var html = "";
    html += "The process <b>" + window.process.name.substr(0,20) + "</b> (with pid " + window.process.pid + ") is using approximately " + formatKB(pss) + " of memory.<br>";
    html += "It is using " + formatKB(private_total) + " privately, and a further " + formatKB(shared_total) + " that is, or could be, shared with other programs.<br>";
    if(shared_total != 0)
        html += "Dividing up the shared memory between all the processes sharing that memory we get a reduced shared memory usage of " + formatKB(pss - private_total) + ". Adding that to the private usage, we get the above mentioned total memory footprint of " + formatKB(pss) + ".<br>";
    if( swap != 0)
        html += swap + " KB is swapped out to disk, probably due to a low amount of available memory left.";

    document.getElementById('processsummary').innerHTML = html;

    getHtmlTableForLibrarySummary(combined, 'Private', private_total);
    getHtmlTableForLibrarySummary(combined, 'Shared', shared_total);

    html = "";
    html += "<tr><th class='memory definedWord' title='Memory used only by this process'>Private</th><td class='memory'>" + private_total + " KB</td><td class='comment'>(= " + private_clean + " KB clean + " + private_dirty + " KB dirty)</td></tr>";
    html += "<tr><th class='memory definedWord' title='Memory that can be used by multiple processes'>Shared</th><td class='memory'>" + shared_total + " KB</td><td class='comment'>(= " + shared_clean + " KB clean + " + shared_dirty + " KB dirty)</td></tr>";
    html += "<tr><th class='memory definedWord' title='Resident Set Size'>Rss</th><td class='memory'>" + rss + " KB</td><td class='comment'>(= Private + Shared)</td></tr>";
    html += "<tr><th class='memory definedWord' title='Proportional Set Size'>Pss</th><td class='memory'>" + pss + " KB</td><td class='comment'>(= Private + Shared/Number of Processes)</td></tr>";
    html += "<tr><th class='memory definedWord' title='Memory swapped out to disk, typically because the system ran low of physical memory'>Swap</th><td class='memory'>" + swap + " KB</td></tr>";
    document.getElementById('totalTableBody').innerHTML = html;
}

function translate() {
    document.getElementById('heading').innerHTML = 'Process ' + window.process.pid + " - " + window.process.name;
    document.getElementById('SummaryHeading').innerHTML = 'Summary';
    document.getElementById('LibraryUsageHeading').innerHTML = 'Library Usage';
    document.getElementById('showFullDetailsLink').innerHTML = 'Show Full Details';
    document.getElementById('libraryusageintro').innerHTML = "The memory usage of a process is found by adding up the memory usage of each of its libraries, plus the process's own heap, stack and any other mappings, plus the stack of any of its threads.";
    document.getElementById('TotalsHeading').innerHTML = "Totals";
    document.getElementById('FullDetailsHeading').innerHTML = "Full Details";
    document.getElementById('linkPrivate').innerHTML = "more";
    document.getElementById('linkShared').innerHTML = "more";
    document.getElementById('thPrivate').innerHTML = "Private";
    document.getElementById('thShared').innerHTML = "Shared";
}
function refresh() {
    var smapData = parseSmaps();
    if(!smapData)
        return;
    var data = smapData[0];
    var combined = smapData[1];

    getHtmlSummary(combined);

    var html = "";
    html += "Information about the complete virtual space for the process is available, with sortable columns.  An empty filename means that it is an <span title='This is a fast way to allocate and clear a block of space.   See the mmap(2) man page under MAP_ANONYMOUS' class='definedWord'>anonymous mapping</span>.<br>";
    if(kernelPageSize != -1 && mmuPageSize != -1 && kernelPageSize == mmuPageSize)
        html += "Both the <span class='definedWord' title='Memory Management Unit'>MMU</span> page size and the kernel page size are " + kernelPageSize + " KB.";
    else {
        if(kernelPageSize != -1)
            html += "The kernel page size is " + kernelPageSize + " KB. ";
        if(mmuPageSize != -1)
            html += "The MMU page size is " + mmuPageSize + " KB.";
    }

    document.getElementById('fullDetailsSummary').innerHTML = html;

    html = "<thead><tr><th>Address</th><th>Perm</th>";
    for(var i = 0; i < sizeKeys.length; i++)
        html += "<th>" + sizeKeys[i].replace('_',' ') + "</th>";
    html += "<th align='left'>Filename</th></tr></thead><tbody>"
    for(var i = 0; i < data.length; i++) {
        html += "<tr><td class='address'>" + data[i]['address'] + "</td><td class='perms'>" + data[i]['perms'] + "</td>";
        for(var j = 0; j < sizeKeys.length; j++) {
            var value = data[i][sizeKeys[j]];
            html += "<td align='right' sorttable_customkey='" + value + "'>" + value + " KB</td>";
        }
        html += "<td>" + data[i]['pathname'] + "</td></tr>";
    }
    html += "</tbody>";

    document.getElementById('fullDetails').innerHTML = html;
}

