////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  NSString+Extensions.m
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "NSString+Extensions.h"
#import <CommonCrypto/CommonDigest.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation NSString (Extensions)

+ (NSString *)safeStringWithString:(NSString *)aString
{
    if (nil != aString) 
    {
        return [NSString stringWithString:aString];
    }
    else
    {
        return [NSString string];
    }
}

+ (id)stringWithUTF8Data:(NSData *)data
{
    return [[[[self class] alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
}

+ (NSString *)stringFromFileSize:(unsigned long long)fileSize precision:(NSInteger)precision
{
    static NSArray *sSufixes = nil;
    
    if (sSufixes == nil)
    {
        sSufixes = [[NSArray alloc] initWithObjects:
            NSLocalizedString(@"B", @"B"),
            NSLocalizedString(@"KB", @"KB"),
            NSLocalizedString(@"MB", @"MB"),
            NSLocalizedString(@"GB", @"GB"),
            NSLocalizedString(@"TB", @"TB"), 
            NSLocalizedString(@"PB", @"PB"), nil];
    }
            
    double size = fileSize;
    NSInteger mult = 0;
    
    while (size >= 1024)
    {
        size /= 1024;
        mult++;
    }
    
    NSString *format = [NSString stringWithFormat:@"%%.%ldf %%@", (long)precision];
    NSString *result = [NSString stringWithFormat:format, size, [sSufixes objectAtIndex:mult]];

    return result;
}

- (BOOL)isEmpty
{
    return 0 == self.length;
}

- (BOOL)isNotEmpty
{
    return 0 < self.length;
}

- (NSUInteger)wordCount
{
    NSUInteger wordCount = 0;
	NSScanner *scanner   = [NSScanner scannerWithString:self];
    
    if (0 < [[scanner string] length])
    {
        [scanner setCharactersToBeSkipped:[NSCharacterSet illegalCharacterSet]];
        
        NSCharacterSet *wordSeparators = [NSCharacterSet whitespaceAndNewlineCharacterSet];
        while(![scanner isAtEnd])
        {
            //	Change scan location to the first character after the word separator
            [scanner scanCharactersFromSet:wordSeparators intoString:NULL];
            
            // Scan up to the first occurrence of any word separator
            if ([scanner scanUpToCharactersFromSet:wordSeparators intoString:NULL])
            {
            	++wordCount;                
            }
        }
    }
    
    return wordCount;
}

- (NSString *)substringWithFirstWordAmount:(NSUInteger)aNeededWordAmount
{
    NSUInteger wordCount = 0;
	NSScanner *scanner   = [NSScanner scannerWithString:self];
    
    if (0 < [[scanner string] length])
    {
        [scanner setCharactersToBeSkipped:[NSCharacterSet illegalCharacterSet]];
        
        while(![scanner isAtEnd] && wordCount < aNeededWordAmount)
        {
            //	Change scan location to the first character after the word divider
            [scanner scanCharactersFromSet:[NSCharacterSet whitespaceAndNewlineCharacterSet] intoString:NULL];

            // Scan up to the first occurrence of any word divider
            if ([scanner scanUpToCharactersFromSet:[NSCharacterSet whitespaceAndNewlineCharacterSet] intoString:NULL])
            {
            	++wordCount;                
            }
        }
    }
    
    return [[scanner string] substringToIndex:[scanner scanLocation]];	
}

- (NSString *)stringByTrimmingSpaces
{
    return [self stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@" "]];
}

- (NSString *)stringByTrimmingWhitespaces
{
    return [self stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
}

- (NSString *)stringByTrimmingWhiteSpacesOnTaill
{
    NSMutableString *result = [NSMutableString stringWithString:self];
    
    while ( ([result length] > 0) && ([result characterAtIndex:[result length] - 1] == ' ') )
    {
        [result deleteCharactersInRange:NSMakeRange([result length] - 1, 1)];
    }
    return [NSString stringWithString:result];
}

- (NSArray *)nonEmptyComponentsSeparatedByCharactersInSet:(NSCharacterSet *)aCharacterSet
{
    return [[self componentsSeparatedByCharactersInSet:aCharacterSet] filteredArrayUsingPredicate:
            [NSPredicate predicateWithBlock:^BOOL(id evaluatedObject, NSDictionary *bindings) 
             {
                 NSString *stringObject = (NSString *)evaluatedObject;
                 return (![stringObject isEqualToString:@""] && 
                         ![[stringObject stringByTrimmingSpaces] isEqualToString:@""]);
             }]];
}

- (NSString *)stringByTrimmingWhiteSpacesAndNewlinesOnTail
{
    NSMutableString *result = [NSMutableString stringWithString:self];
    
    while ( ([result length] > 0) && 
           ([result characterAtIndex:[result length] - 1] == ' ' || [result characterAtIndex:[result length] - 1] == '\n') )
    {
        [result deleteCharactersInRange:NSMakeRange([result length] - 1, 1)];
    }
    return [NSString stringWithString:result];
}

- (NSString *)stringByTrimming0D0A09Sequences
{
    NSMutableString *result = [NSMutableString stringWithString:self];
    
    NSUInteger index = 0;
    while (true)
    {
        // Find 0D 0A sequence
        NSRange range = [result rangeOfString:@" \n" options:0 range:NSMakeRange(index, [result length] - index)];
        if (range.location == NSNotFound)
        {
            range = [result rangeOfString:@"\n" options:0 range:NSMakeRange(index, [result length] - index)];
            if (range.location == NSNotFound)
            {
                break;
            }
        }
        
        // Make sure it is followed by 09
        NSUInteger lastTabIndex = range.location + range.length - 1;
        while ([result length] > lastTabIndex + 1 && 
               [result characterAtIndex:lastTabIndex + 1] == '\t')
        {
            lastTabIndex ++;
        }
        
        if (lastTabIndex != range.location + range.length - 1)
        {
            // We found the sequence, remove
            NSRange rangeToRemove = NSMakeRange(range.location, lastTabIndex - range.location + 1);
            [result deleteCharactersInRange:rangeToRemove];
        }
        else
        {
            index += range.length;
        }
    }
    
    return [NSString stringWithString:result];
}

- (NSString *)stringBytrimmingBlanks
{
    return [self stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@" \t\n\r"]];
}

- (NSString *)stringByDeletingCharactersInSet:(NSCharacterSet *)aSet
{
    NSParameterAssert(nil != aSet);
    
    return [[self componentsSeparatedByCharactersInSet:aSet] componentsJoinedByString:@""];
}

- (NSString *)stringByDeletingPunctuationCharacters
{
    return [self stringByDeletingCharactersInSet:[NSCharacterSet punctuationCharacterSet]];
}

- (NSData *)hexToBytes
{
    NSMutableData *data = [NSMutableData data];
    
    for (NSUInteger index = 0; index + 2 <= self.length; index += 2)
    {
        NSRange range = NSMakeRange(index, 2);
        NSString *hexStr = [self substringWithRange:range];
        NSScanner *scanner = [NSScanner scannerWithString:hexStr];
        unsigned int intValue = 0;

        [scanner scanHexInt:&intValue];
        [data appendBytes:&intValue length:1];
    }
    
    return data;
}

- (NSComparisonResult)compareNumeric:(NSString *)string
{
    return [self compare:string options:NSCaseInsensitiveSearch | NSNumericSearch];
}

+ (NSString *)urlEncode:(NSString *)anUrl
{
    NSArray *escapeChars = [NSArray arrayWithObjects:@";" , @"/" , @"?" , @":" ,
													 @"@" , @"&" , @"=" , @"+" ,
													 @"$" , @"," , @"[" , @"]",
													 @"#", @"!", @"'", @"(", 
													 @")", @"*", nil];
	
    NSArray *replaceChars = [NSArray arrayWithObjects:@"%3B" , @"%2F" , @"%3F" ,
													  @"%3A" , @"%40" , @"%26" ,
													  @"%3D" , @"%2B" , @"%24" ,
													  @"%2C" , @"%5B" , @"%5D", 
													  @"%23", @"%21", @"%27",
													  @"%28", @"%29", @"%2A", nil];
	
    NSUInteger len = [escapeChars count];
    NSMutableString *temp = [[anUrl mutableCopy] autorelease];
	
    for(int i = 0; i < len; i++)
    {
        [temp replaceOccurrencesOfString:[escapeChars objectAtIndex:i]
							  withString:[replaceChars objectAtIndex:i]
								 options:NSLiteralSearch
								   range:NSMakeRange(0, [temp length])];
    }
	
    return [NSString stringWithString:temp];
}


- (BOOL)containsAllWordsInString:(NSString*)aString options:(NSStringCompareOptions)aOptions
{
    if (nil == aString || [aString length] == 0)
    {
        return YES;
    }
    
    return ([[self rangesOfAllWordsInString:aString options:aOptions] count] > 0);
}

- (NSArray *)rangesOfAllWordsInString:(NSString*)aString options:(NSStringCompareOptions)aOptions
{
    // get all words from given string
    NSArray *words = [aString componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    
    if (nil == words || [words count] == 0)
    {
        return [NSArray array];
    }
    
    // remove duplicates preserving words order and drop empty words
    NSMutableArray *wordsWithoutDuplicates = [NSMutableArray arrayWithCapacity:[words count]];
    
    for (NSUInteger i = 0; i < [words count]; ++i)
    {
        NSString *word = [words objectAtIndex:i];
        
        if ([word length] > 0 && [words indexOfObject:word] == i)
        {
            [wordsWithoutDuplicates addObject:[words objectAtIndex:i]];
        }
    }
    
    NSMutableArray *mutableResult = [NSMutableArray array];
    
    // add ranges for each word
    for (NSString *word in wordsWithoutDuplicates)
    {
        BOOL wordIsPresent = NO;
        
        NSArray *wordRanges = [self rangesOfString:word options:aOptions];
        
        // check for overlapping with existing ranges
        for (NSValue *wordRange in wordRanges)
        {
            BOOL includeRange = YES;
            
            for (NSValue *existingRangeValue in mutableResult)
            {
                if (NSIntersectionRange([wordRange rangeValue], [existingRangeValue rangeValue]).length > 0)
                {
                    includeRange = NO;
                    break;
                }
            }
            
            if (includeRange)
            {
                [mutableResult addObject:wordRange];
                wordIsPresent = YES;
            }
        }
        
        // return empty array if one of the words is missing
        if (!wordIsPresent)
        {
            return [NSArray array];
        }
    }
    
    return [NSArray arrayWithArray:mutableResult];
}

- (NSArray *)rangesOfString:(NSString*)aString options:(NSStringCompareOptions)aOptions
{
    if (nil == aString|| [aString length] == 0)
    {
        return [NSArray array];
    }
    
    NSMutableArray *mutableResult = [NSMutableArray array];
    
    NSRange searchRange = NSMakeRange(0, [self length]);
    for (;;)
    {
        NSRange substringRange = [self rangeOfString:aString options:aOptions range:searchRange];
        if (substringRange.location == NSNotFound)
        {
            break;
        }
        else
        {
            [mutableResult addObject:[NSValue valueWithRange:substringRange]];
            searchRange.location = NSMaxRange(substringRange);
            searchRange.length = [self length] - searchRange.location;
        }
    }
    
    return [NSArray arrayWithArray:mutableResult];
}

- (NSUInteger)longestWordLength
{
    // simple not optimized imlementation
    
    NSUInteger result = 0;
    
    NSArray *words = [self componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    
    for (NSString *word in words)
    {
        NSUInteger wordLength = [word length];
        result = MAX(result, wordLength);
    }
    
    return result;
}

- (NSString *)stringByEscapingTriangularBrackets
{
    return [[self stringByReplacingOccurrencesOfString:@"<" withString:@"&lt;"] 
        stringByReplacingOccurrencesOfString:@">" withString:@"&gt;"];
}

- (NSString *)stringByUnescapingTriangularBrackets
{
    return [[self stringByReplacingOccurrencesOfString:@"&lt;" withString:@"<"] 
        stringByReplacingOccurrencesOfString:@"&gt;" withString:@">"];    
}

- (NSString *) md5
{
    const char *cStr = [self UTF8String];
    unsigned char result[16];
    CC_MD5(cStr, (CC_LONG)strlen(cStr), result);
    return [NSString stringWithFormat:
            @"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            result[0], result[1], result[2], result[3],
            result[4], result[5], result[6], result[7],
            result[8], result[9], result[10], result[11],
            result[12], result[13], result[14], result[15]
            ];
}

@end
