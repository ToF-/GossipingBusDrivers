import Data.List
import Data.Map (Map)
import qualified Data.Map as M
import Data.Set (Set)
import qualified Data.Set as S
import Data.Function (on)
import Data.Maybe (catMaybes)

newtype Stop   = Stop Int
    deriving (Eq,Ord)

newtype Route  = Route [Stop]

newtype Driver = Driver Int
    deriving (Eq,Ord)

newtype Minute = Minute Int

type Knowledge = Map Driver (Set Driver)

routes :: String -> [Route]
routes = map (Route . map (Stop . read) . words) . lines

stopTable :: [Route] -> [[Stop]]
stopTable = transpose . map (\(Route stops) -> take 480 (cycle stops))

initial :: Int -> Knowledge
initial maxRoutes = M.fromList [(Driver i, S.fromList [Driver i])
                               | i <- [0..maxRoutes-1]]

complete :: Knowledge -> Bool
complete k = all (\g -> S.size g == M.size k) (M.elems k)

type Connection = [Driver]

connections :: [Stop] -> [Connection]
connections = filter ((>1) . length)
            . map (map snd)
            . groupBy (on (==) fst)
            . sort
            . flip zip (map Driver [0..])

update :: Knowledge -> Connection -> Knowledge
update knowledge c = foldl updateDriver knowledge c
    where
    updateDriver :: Knowledge -> Driver -> Knowledge
    updateDriver k d = M.insert d sharedGossips k

    sharedGossips :: Set Driver
    sharedGossips = S.unions $ catMaybes [M.lookup d knowledge | d <- c]

minuteToCompleteUpdate :: [Route] -> Maybe Minute
minuteToCompleteUpdate rs = updateStops (Minute 0) (initial (length rs)) (stopTable rs)
    where
    updateStops :: Minute -> Knowledge -> [[Stop]] -> Maybe Minute
    updateStops m k _ | complete k = Just m
    updateStops _ k [] = Nothing
    updateStops (Minute n) k (stops:next) = updateStops (Minute (succ n)) k' next
        where k' = foldl update k (connections stops)

main :: IO ()
main = do
    rs <- routes <$> getContents
    case minuteToCompleteUpdate rs of
        Nothing -> putStrLn "never"
        Just (Minute n)  -> print n

