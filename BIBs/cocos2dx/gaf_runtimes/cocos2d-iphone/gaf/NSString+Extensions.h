@interface NSString (Extensions)

+ (NSString *)safeStringWithString:(NSString *)aString;

+ (id)stringWithUTF8Data:(NSData *)data;
+ (NSString *)stringFromFileSize:(unsigned long long)fileSize precision:(NSInteger)precision;

- (BOOL)isEmpty;
- (BOOL)isNotEmpty;

- (NSUInteger)wordCount;

///	Returns a substring that contains the first aNeededWordAmount words from the receiver.
/// Returns a whole string if the receiver has less then aNeededWordAmount words.
- (NSString *)substringWithFirstWordAmount:(NSUInteger)aNeededWordAmount;

- (NSString *)stringByTrimmingSpaces;
- (NSString *)stringByTrimmingWhitespaces;
- (NSString *)stringByTrimmingWhiteSpacesOnTaill;
- (NSString *)stringByTrimmingWhiteSpacesAndNewlinesOnTail;
- (NSString *)stringBytrimmingBlanks;

- (NSString *)stringByTrimming0D0A09Sequences;

- (NSArray *)nonEmptyComponentsSeparatedByCharactersInSet:(NSCharacterSet *)aCharacterSet;

- (NSString *)stringByDeletingCharactersInSet:(NSCharacterSet *)aSet;
- (NSString *)stringByDeletingPunctuationCharacters;

- (NSData *)hexToBytes;

- (NSComparisonResult)compareNumeric:(NSString *)string;
+ (NSString *)urlEncode:(NSString *)anUrl;

- (BOOL)containsAllWordsInString:(NSString*)aString options:(NSStringCompareOptions)aOptions;

// Returns array of NSValue* with ranges.
// Returns non-empty array only if all words are present.
// If one of the words is absent (though other may be present) - returns empty array.
// Not optimized for performance - intended to use with small number of words in string.
- (NSArray *)rangesOfAllWordsInString:(NSString*)aString options:(NSStringCompareOptions)aOptions;

// Returns array of NSValue* with ranges.
- (NSArray *)rangesOfString:(NSString*)aString options:(NSStringCompareOptions)aOptions;

// Returns length of longest word in string.
- (NSUInteger)longestWordLength;

- (NSString *)stringByEscapingTriangularBrackets;
- (NSString *)stringByUnescapingTriangularBrackets;

- (NSString *)md5;

@end
